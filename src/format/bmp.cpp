#include "format/bmp.hpp"
#include "image.hpp"
#include "numbers.hpp"

#include <vector>
#include <ranges>
#include <cstdint>
#include <bit>
#include <istream>
#include <ostream>

template <Index Bytes>
struct Word_impl {};

template <> struct Word_impl<1> { using type = uint8_t; };
template <> struct Word_impl<2> { using type = uint16_t; };
template <> struct Word_impl<4> { using type = uint32_t; };

template <Index Bytes>
using Word = typename Word_impl<Bytes>::type;

static_assert(sizeof(float) == 4, "require 32-bit floats");

template <typename Ty, typename BTy>
Ty narrow(BTy x) { return static_cast<Ty>(x); }

template <typename Ty>
void readValue(std::istream& is, Ty& payload)
{
    static constexpr Index size = sizeof(Ty);
    Word<1> buffer[size] {};
    Word<size> bits = 0;
    if (is.read(reinterpret_cast<char*>(buffer), size))
    {
        for (Index i = 0; i < size; i++)
            bits |= buffer[i] << (8 * i);
        payload = std::bit_cast<Ty>(bits);
    }
}

template <typename Ty>
void writeValue(std::ostream& os, const Ty& payload)
{
    static constexpr Index size = sizeof(Ty);
    Word<1> buffer[size];
    Word<size> bits = std::bit_cast<Word<size>, Ty>(payload);
    for (Index i = 0; i < size; i++)
        buffer[i] = narrow<Word<1>>(bits >> (8 * i));
    os.write(reinterpret_cast<char*>(buffer), size);
}

struct Header
{
    static constexpr Word<2> signature = 0x4D42; //BM
    Word<4> fileSize;
    float maxLuminance; // normally unused
    Word<4> dataOffset;

    void read(std::istream& is)
    {
        readValue(is, fileSize);
        readValue(is, maxLuminance);
        readValue(is, dataOffset);
    }
    
    void write(std::ostream& os)
    {
        writeValue(os, fileSize);
        writeValue(os, maxLuminance);
        writeValue(os, dataOffset);
    }
};

struct InfoHeader
{
    Word<4> size, width, height;
    Word<2> planes, bitcount;
    Word<4> compression, imageSize; //compressed size; 0 if compression == 0
    Word<4> unused[4];

    void read(std::istream& is)
    {
        readValue(is, size); readValue(is, width);
        readValue(is, height); readValue(is, planes);
        readValue(is, bitcount); readValue(is, compression);
        readValue(is, imageSize);
        for (auto i = 0; i < 4; i++)
            readValue(is, unused[i]);
    }

    void write(std::ostream& os)
    {
        writeValue(os, size); writeValue(os, width);
        writeValue(os, height); writeValue(os, planes);
        writeValue(os, bitcount); writeValue(os, compression);
        writeValue(os, imageSize);
        for (auto i = 0; i < 4; i++)
            writeValue(os, unused[i]);
    }
};

struct ColorTableEntry
{
    Word<1> red, green, blue, unused;

    void read(std::istream& is)
    {
        readValue(is, red); readValue(is, green);
        readValue(is, blue); readValue(is, unused);
    }
};

//using ColorTable = std::vector<ColorTableEntry>;

template <typename WordT>
bool extractPixel(std::istream& is, const Image& img, RGBPixel& pixel, Index j)
{
    auto inputValue = [&](Natural s) -> Real
        { return (s * PrecisionReal{img.luminance()}) / img.resolution(); };

    static constexpr Natural wordsize = sizeof(WordT) * 8;

    static Natural recordedBits = 0; // State machine per row

    WordT red = 0, green = 0, blue = 0;
    readValue(is, blue); if (!is) return false;
    readValue(is, green); if (!is) return false;
    readValue(is, red); if (!is) return false;

    pixel.r = inputValue(red);
    pixel.g = inputValue(green);
    pixel.b = inputValue(blue);

    recordedBits += 3 * wordsize;
    if (j + 1 == img.dimensions().width)
    {
        Natural paddingBytes = (4 - (recordedBits % 32) / 8) % 4;
        is.seekg(paddingBytes, std::istream::cur);
        recordedBits = 0;
    }
    
    return true;
}

bool bmp::read(std::istream& is, Image& img)
{
    Header header {};
    InfoHeader info {};
    //ColorTable colorTable;

    header.read(is); 
    info.read(is);

    // In this project only 24 bit RGB (and custom extension for HDR) is accepted
    if (info.bitcount != 24 && info.bitcount != 48 && info.bitcount != 96)
        return false;

    auto bitsPerElem = info.bitcount <= 8 ? info.bitcount : info.bitcount / 3;
    Real luminance = header.maxLuminance == 0 ? 1 : header.maxLuminance;

    img = Image{luminance, Natural{1} << bitsPerElem , Dimensions{info.width, info.height}};

    // Only uncompressed files accepted
    if (info.compression != 0)
        return false;

    is.seekg(header.dataOffset);

    auto extractFunction = &extractPixel<Word<1>>;
    if (bitsPerElem == 16) extractFunction = &extractPixel<Word<2>>;
    else if (bitsPerElem == 32) extractFunction = &extractPixel<Word<4>>;

    auto [width, height] = img.dimensions();
    for (Index i : numbers::range(0, height) | std::views::reverse)
    for (Index j : numbers::range(0, width))
    {
        RGBPixel pixel;
        if (!extractFunction(is, img, pixel, j))
            return false;
        img(i, j) = pixel;
    }
            
    return true;
}

template <typename WordT>
void insertPixel(std::ostream& os, const Image& img, const RGBPixel& pixel, Index j)
{
    auto outputValue = [&](Real v) -> Natural
    {
        Natural val = std::round(v * (img.resolution() / PrecisionReal{img.luminance()}));
        return numbers::min(val, img.resolution());
    };

    static constexpr Natural wordsize = sizeof(WordT) * 8;

    static Natural recordedBits = 0;

    WordT blue = outputValue(pixel.b);
    WordT green = outputValue(pixel.g);
    WordT red = outputValue(pixel.r);

    writeValue(os, blue);
    writeValue(os, green);
    writeValue(os, red);

    recordedBits += 3 * wordsize;
    if (j + 1 == img.dimensions().width)
    {
        Natural paddingBytes = (4 - (recordedBits % 32) / 8) % 4;
        Word<1> padding[3] {0, 0, 0};
        os.write(reinterpret_cast<char*>(padding), paddingBytes);
        recordedBits = 0;
    }
}

bool bmp::write(std::ostream& os, const Image& img)
{
    auto [width, height] = img.dimensions();

    auto msb = [](Natural x) { Natural r{}; while (x >>= 1) r++; return r; };

    if (img.resolution() != std::numeric_limits<Word<1>>::max()
        && img.resolution() != std::numeric_limits<Word<2>>::max()
        && img.resolution() != std::numeric_limits<Word<4>>::max())
        return false;

    const Natural bitsPerPixel = (msb(img.resolution()) + 1) * 3;
    const Natural bitsPerRow = width * bitsPerPixel;
    const Natural paddingPerRow = (32 - (bitsPerRow % 32)) % 32;
    const Natural rowSize = (bitsPerRow + paddingPerRow) / 8;
    const Natural colorTableSize = 0; // not going to implement
    const Natural fileSize = 54 + height * rowSize + colorTableSize;

    Header header
    {
        .fileSize = narrow<Word<4>>(fileSize),
        .maxLuminance = narrow<float>(img.luminance()),
        .dataOffset = narrow<Word<4>>(54 + colorTableSize)
    };

    InfoHeader info
    {
        .size = 40,
        .width = narrow<Word<4>>(width),
        .height = narrow<Word<4>>(height),
        .planes = 1,
        .bitcount = narrow<Word<2>>(bitsPerPixel),
        .compression = 0,
        .imageSize = narrow<Word<4>>(header.fileSize - header.dataOffset),
        .unused = {}
    };

    os.put('B').put('M');
    header.write(os);
    info.write(os);

    os.seekp(header.dataOffset);

    const auto bitsPerElem = info.bitcount / 3;
    auto insertFunction = &insertPixel<Word<1>>;
    if (bitsPerElem == 16) insertFunction = &insertPixel<Word<2>>;
    else if (bitsPerElem == 32) insertFunction = &insertPixel<Word<4>>;

    for (Index i : numbers::range(0, height) | std::views::reverse)
    for (Index j : numbers::range(0, width))
        insertFunction(os, img, img(i, j), j);

    return true;
}
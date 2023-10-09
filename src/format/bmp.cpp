#include "format/bmp.hpp"
#include "image.hpp"
#include "numbers.hpp"

#include <vector>
#include <ranges>

#include <iostream>

using _1byte = uint8_t;
using _2byte = uint16_t;
using _4byte = uint32_t;

struct __attribute__ ((packed)) Header
{
    static constexpr _2byte signature = 0x4D42; //BM
    _4byte fileSize;
    _Float32 maxLuminance; // normally unused
    _4byte dataOffset;
};

struct __attribute__ ((packed)) InfoHeader
{
    _4byte size;
    _4byte width;
    _4byte height;
    _2byte planes;
    _2byte bitcount;
    _4byte compression;
    _4byte imageSize; //compressed size; 0 if compression == 0
    _4byte unused[4];
};

struct __attribute__ ((packed)) ColorTableEntry
{
    _1byte red;
    _1byte green;
    _1byte blue;
    _1byte unused;
};

using ColorTable = std::vector<ColorTableEntry>;

template <typename Ty>
void readValue(std::istream& is, Ty& value)
{
    is.read(reinterpret_cast<char*>(&value), sizeof(Ty));
}

template <typename WordT>
inline bool extractColor(std::istream& is, WordT& word, WordT& color,
        Natural bitcount, Natural& extracted)
{
    //Extracts and shifts n bits form byte
    auto getbits = [](WordT& byte, Natural n) -> WordT
    {
        WordT res = byte & ((1 << n) - 1);
        byte = byte >> n;
        return res;
    };

    static constexpr Natural wordsize = sizeof(WordT) * 8;

    Natural remaining = bitcount;

    auto step = [&]()
    {
        if (extracted == 0)
        {
            readValue(is, word);
            if (!is) return false;
        }

        Natural bits = numbers::min(remaining, wordsize - extracted);
        color = getbits(word, bits);

        remaining = remaining - bits;
        extracted = (extracted + bits) % wordsize;

        return true;
    };

    return step() && (remaining == 0 || step());
}

struct Extractor
{
    virtual bool operator()(RGBPixel& pixel, Index j) = 0;
};

// State machine per row
template <typename WordT>
struct DirectExtractor : Extractor
{
    std::istream& is;
    const Natural bitcount;
    const Image& img;

    DirectExtractor(std::istream& is_, Natural bc_, const Image& img_)
        : is{is_}, bitcount{bc_}, img{img_}
    {}

    virtual bool operator()(RGBPixel& pixel, Index j) override
    {
        auto inputValue = [&](Natural s) -> Real
            { return s * (img.luminance() / img.resolution()); };
        
        static Natural extracted = 0;
        static WordT word = 0;
        static Natural recordedBits = 0;

        WordT red, green, blue;
        if (!extractColor(is, word, red, bitcount / 3, extracted)) return false;
        if (!extractColor(is, word, green, bitcount / 3, extracted)) return false;
        if (!extractColor(is, word, blue, bitcount / 3, extracted)) return false;

        pixel.r = inputValue(red);
        pixel.g = inputValue(green);
        pixel.b = inputValue(blue);

        recordedBits += bitcount;
        auto dim = img.dimensions();
        if (j + 1 == dim.width)
        {
            Natural paddingBytes = (recordedBits % 32) / 8;
            is.seekg(paddingBytes, std::istream::cur);
            recordedBits = 0; extracted = 0;
        }
        
        return true;
    }
};

struct IndirectExtractor : Extractor
{
    using WordT = _1byte;

    std::istream& is;
    const Natural bitcount;
    const Image& img;
    const ColorTable& colorTable;

    IndirectExtractor(std::istream& is_, Natural bc_,
            const Image& img_, const ColorTable& ct_)
        : is{is_}, bitcount{bc_}, img{img_}, colorTable{ct_}
    {}

    virtual bool operator()(RGBPixel& pixel, Index j) override
    {
        auto inputValue = [&](Natural s) -> Real
            { return s * (img.luminance() / img.resolution()); };

        static Natural extracted = 0;
        static WordT word = 0;
        static Natural recordedBits = 0;

        WordT index;
        if (!extractColor(is, word, index, bitcount, extracted)) return false;

        auto entry = colorTable[index];

        pixel.r = inputValue(entry.red);
        pixel.g = inputValue(entry.green);
        pixel.b = inputValue(entry.blue);

        recordedBits += bitcount;
        auto dim = img.dimensions();
        if (j + 1 == dim.width)
        {
            Natural paddingBytes = (recordedBits % 32) / 8;
            is.seekg(paddingBytes, std::istream::cur);
            recordedBits = 0; extracted = 0;
        }
        
        return true;
    }
};

template <typename WordT>
struct FixedExtractor : Extractor
{
    std::istream& is;
    const Image& img;

    FixedExtractor(std::istream& is_, const Image& img_)
        : is{is_}, img{img_}
    {}

    virtual bool operator()(RGBPixel& pixel, Index j) override
    {
        auto inputValue = [&](Natural s) -> Real
            { return (s * img.luminance()) / img.resolution(); };

        static constexpr Natural wordsize = sizeof(WordT) * 8;

        static Natural recordedBits = 0;

        WordT red, green, blue;
        readValue(is, blue); if (!is) return false;
        readValue(is, green); if (!is) return false;
        readValue(is, red); if (!is) return false;

        // std::cout << "{" << (Natural)red << ", " << (Natural)green << ", " << (Natural)blue << "}\t\t";

        pixel.r = inputValue(red);
        pixel.g = inputValue(green);
        pixel.b = inputValue(blue);

        // std::cout << "luminance: " << img.luminance() << "\t\t";
        // std::cout << "{" << pixel.r << ", " << pixel.g << ", " << pixel.b << "}\n";

        recordedBits += 3 * wordsize;
        auto dim = img.dimensions();
        if (j + 1 == dim.width)
        {
            Natural paddingBytes = (4 - (recordedBits % 32) / 8) % 4;
            is.seekg(paddingBytes, std::istream::cur);
            // std::cout << "RecordedBits: " << recordedBits << "\n";
            // std::cout << "Padding: " << paddingBytes << "\n";
            recordedBits = 0;
        }
        
        return true;
    }
};

bool bmp::read(std::istream& is, Image& img)
{
    Header header;
    InfoHeader info;
    ColorTable colorTable;

    readValue(is, header);
    readValue(is, info);

    std::cout << "sizeof(Header): " << sizeof(Header) << '\n';
    std::cout << "sizeof(InfoHeader): " << sizeof(InfoHeader) << '\n';
    std::cout << "sizeof(ColorTableEntry): " << sizeof(ColorTableEntry) << '\n';
    std::cout << '\n';
    std::cout << "HEADER: \n";
    std::cout << "fileSize: " << header.fileSize << '\n';
    std::cout << "maxLuminance: " << header.maxLuminance << '\n';
    std::cout << "dataOffset: " << header.dataOffset << '\n';
    std::cout << '\n';
    std::cout << "INFO: \n";
    std::cout << "size: " << info.size << '\n';
    std::cout << "width: " << info.width << '\n';
    std::cout << "height: " << info.height << '\n';
    std::cout << "planes: " << info.planes << '\n';
    std::cout << "bitcount: " << info.bitcount << '\n';
    std::cout << "compression: " << info.compression << '\n';
    std::cout << "imageSize: " << info.imageSize << '\n';

    if (info.bitcount % 3 != 0)
        return false;

    auto bitsPerElem = info.bitcount <= 8 ? info.bitcount : info.bitcount / 3;
    Real luminance = header.maxLuminance == 0 ? 1 : header.maxLuminance;

    img = Image{luminance, Natural{1} << bitsPerElem , Dimensions{info.width, info.height}};

    // Only uncompressed files accepted
    if (info.compression != 0)
        return false;

    auto loopRaster = [&](Extractor& extract)
    {
        auto [width, height] = img.dimensions();
        for (Index i : std::views::iota(Index{0}, height) | std::views::reverse)
        {
            for (Index j : std::views::iota(Index{0}, width))
            {
                RGBPixel pixel;
                if (!extract(pixel, j))
                    return false;
                img(i, j) = pixel;
            }
        }
               
        return true;
    };
  
    if (info.bitcount <= 8)
    {
        for (Index i = 0; i < img.resolution(); i++)
        {
            if (header.dataOffset - is.tellg() < 4)
                return false;

            ColorTableEntry entry;
            readValue(is, entry);
            colorTable.push_back(entry);
        }
        is.seekg(header.dataOffset);
        
        IndirectExtractor extractor {is, info.bitcount, img, colorTable};
        return loopRaster(extractor);
    }

    //is.seekg(header.dataOffset - 2);

    std::cout << '\n';
    std::cout << "Cuurrent position: " << is.tellg() << '\n';

    if (bitsPerElem < 8)
    {
        DirectExtractor<uint8_t> extractor {is, info.bitcount, img};
        return loopRaster(extractor);
    }
    else if (bitsPerElem == 8)
    {
        std::cout << "Using FixedExtractor<8bits>\n";
        FixedExtractor<uint8_t> extractor {is, img};
        return loopRaster(extractor);
    }
    else if (bitsPerElem < 16)
    {
        DirectExtractor<uint16_t> extractor {is, info.bitcount, img};
        return loopRaster(extractor);
    }
    else if (bitsPerElem == 16)
    {
        FixedExtractor<uint16_t> extractor {is, img};
        return loopRaster(extractor);
    }
    else if (bitsPerElem < 32)
    {
        DirectExtractor<uint32_t> extractor {is, info.bitcount, img};
        return loopRaster(extractor);
    }
    else if (bitsPerElem == 32)
    {
        FixedExtractor<uint32_t> extractor {is, img};
        return loopRaster(extractor);
    }
    return false;
}

void bmp::write(std::ostream& /*os*/, const Image& /*img*/)
{

}
#pragma once

#include <fstream>
#include <memory>

#include "image.hpp"

class ImageReader
{
protected:
    std::ifstream is;
public:
    ImageReader(std::ifstream&& is_) : is(std::move(is_)) {}

    [[nodiscard]] virtual bool read(Image& img) = 0;
};

class PPMReader : public ImageReader
{
public:
    PPMReader(std::ifstream&& is_) : ImageReader(std::move(is_)) {}

    [[nodiscard]] virtual bool read(Image& img) override { return ppm::read(is, img); }
};

[[nodiscard]] std::unique_ptr<ImageReader> makeImageReader(std::string_view path)
{
    std::ifstream is{std::string{path}, std::ios::binary};
    if (!is.is_open())
        return nullptr;

    uint16_t magicNumber;
    is.read(reinterpret_cast<char*>(&magicNumber), 2);

    switch (magicNumber)
    {
    case 0x3350: return std::make_unique<PPMReader>(std::move(is)); //P3
    //case 0x4D42: return std::make_unique<BMPReader>(is); //BM
    default:
        return nullptr;
    }
}

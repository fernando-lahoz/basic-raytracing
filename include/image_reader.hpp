#pragma once

#include <fstream>
#include <filesystem>
#include <memory>

#include "image.hpp"

class ImageReader
{
protected:
    std::ifstream is;

    ImageReader(std::ifstream&& is_) : is(std::move(is_)) {}
public:
    virtual [[nodiscard]] bool read(Image& img) = 0;

    friend std::unique_ptr<ImageReader> openImageReader(std::filesystem::path path);
};

class PPMReader : public ImageReader
{
private:
    PPMReader(std::ifstream&& is_) : ImageReader(std::move(is_)) {}
public:
    virtual [[nodiscard]] bool read(Image& img) override { ppm::read(is, img); }

    friend std::unique_ptr<ImageReader> openImageReader(std::filesystem::path path);
};

[[nodiscard]] std::unique_ptr<ImageReader> openImageReader(std::filesystem::path path)
{
    std::ifstream is{path, std::ios::binary};
    if (!is.is_open())
        return nullptr;

    uint16_t magicNumber;
    is >> magicNumber;

    switch (magicNumber)
    {
    case 0x3050: return std::make_unique<PPMReader>(is); //P3
    //case 0x4D42: return std::make_unique<BMPReader>(is); //BM
    default:
        return nullptr;
    }
}

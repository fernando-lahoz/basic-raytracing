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
    [[nodiscard]] virtual bool read(Image& img) = 0;

    friend std::unique_ptr<ImageReader> makeImageReader(std::filesystem::path path);
};

class PPMReader : public ImageReader
{
private:
    PPMReader(std::ifstream&& is_) : ImageReader(std::move(is_)) {}
public:
    [[nodiscard]] virtual bool read(Image& img) override { return ppm::read(is, img); }

    friend std::unique_ptr<ImageReader> makeImageReader(std::filesystem::path path);
};

[[nodiscard]] std::unique_ptr<ImageReader> makeImageReader(std::filesystem::path path)
{
    std::ifstream is{path, std::ios::binary};
    if (!is.is_open())
        return nullptr;

    uint16_t magicNumber;
    is.read(reinterpret_cast<char*>(&magicNumber), 2);

    switch (magicNumber)
    {
    case 0x3350: return std::unique_ptr<PPMReader>{new PPMReader{std::move(is)}}; //P3
    //case 0x4D42: return std::make_unique<BMPReader>(is); //BM
    default:
        return nullptr;
    }
}

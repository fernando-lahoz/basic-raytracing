#pragma once

#include <fstream>
#include <memory>

#include "image.hpp"
#include "format/ppm.hpp"
#include "format/bmp.hpp"

class ImageWriter
{
protected:
    std::ofstream os;

    ImageWriter(std::ofstream&& os_) : os(std::move(os_)) {}
public:
    virtual void write(const Image& img) = 0;
};

class PPMWriter : public ImageWriter
{
public:
    PPMWriter(std::ofstream&& os_) : ImageWriter(std::move(os_)) {}

    virtual void write(const Image& img) override { ppm::write(os, img); }
};

class BMPWriter : public ImageWriter
{
public:
    BMPWriter(std::ofstream&& os_) : ImageWriter(std::move(os_)) {}

    virtual void write(const Image& img) override { bmp::write(os, img); }
};

template <typename IW = ImageWriter>
[[nodiscard]] std::unique_ptr<ImageWriter> makeImageWriter(std::string_view path, std::string_view format = "")
{
    auto getExtension = [](std::string_view path, std::string_view& extension)
    {
        Index dot = path.find_last_of('.');
        if (dot == std::string_view::npos || dot + 1 == path.length())
            return false;
        extension = path.substr(dot + 1);
        return true;
    };

    std::ofstream os{std::string(path), std::ios::binary};
    if (!os.is_open())
        return nullptr;
    
    if constexpr (std::is_same<IW, ImageWriter>{})
    {
        std::string_view fmtStr = format;
        if (fmtStr.empty() && !getExtension(path, fmtStr))
            return nullptr;

        if (fmtStr == "ppm")
            return std::make_unique<PPMWriter>(std::move(os));
        else if (fmtStr == "bmp")
            return std::make_unique<BMPWriter>(std::move(os));
        return nullptr;
    }
    else {
        return std::make_unique<IW>(std::move(os));
    }
}

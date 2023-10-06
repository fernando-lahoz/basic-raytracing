#pragma once

#include <fstream>
#include <filesystem>
#include <memory>

#include "image.hpp"

class ImageWriter;

template <typename IW>
[[nodiscard]] std::unique_ptr<ImageWriter> makeImageWriter(std::filesystem::path path);

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

template <typename IW = ImageWriter>
[[nodiscard]] std::unique_ptr<ImageWriter> makeImageWriter(std::filesystem::path path, std::string_view format = "")
{
    std::ofstream os{path, std::ios::binary};
    if (!os.is_open())
        return nullptr;
    
    if constexpr (std::is_same<IW, ImageWriter>{})
    {
        if (format.empty())
        {
            format = path.extension().c_str();
            if (format.empty()) return nullptr;
            format = format.substr(1);
        }

        if (format == "ppm")
            return std::make_unique<PPMWriter>(std::move(os));
        // else if ()

        return nullptr;
    }
    else {
        return std::make_unique<IW>(std::move(os));
    }
}

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

    template <typename IW>
    friend std::unique_ptr<ImageWriter> makeImageWriter(std::filesystem::path path);
};

class PPMWriter : public ImageWriter
{
private:
    PPMWriter(std::ofstream&& os_) : ImageWriter(std::move(os_)) {}
public:
    virtual void write(const Image& img) override { ppm::write(os, img); }

    template <typename IW>
    friend std::unique_ptr<ImageWriter> makeImageWriter(std::filesystem::path path);
};

template <typename IW = ImageWriter>
[[nodiscard]] std::unique_ptr<ImageWriter> makeImageWriter(std::filesystem::path path)
{
    std::ofstream os{path, std::ios::binary};
    if (!os.is_open())
        return nullptr;
    
    if constexpr (std::is_same<IW, ImageWriter>{})
    {
        if (path.extension() == ".ppm")
            return std::unique_ptr<PPMWriter>{new PPMWriter{std::move(os)}};
        // else if ()

        return nullptr;
    }
    else {
        return std::make_unique<IW>(os);
    }
}
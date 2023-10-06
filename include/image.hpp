#pragma once

#include <vector>

#include "numbers.hpp"
#include "color_spaces.hpp"

#include "format/ppm.hpp"

class ToneMappingStrategy;

struct Dimensions
{
    Index width, height;
};

class Image
{
private:
    std::vector<Real> redBuffer;
    std::vector<Real> greenBuffer;
    std::vector<Real> blueBuffer;
    
    Real maxLuminance;
    Natural colorResolution;
    Index nColumns, nRows;

public:
    Image() : Image{1, 255, {}} {}

    Image(Real maxLum, Natural colorRes, Dimensions dim)
        : maxLuminance{maxLum}, colorResolution{colorRes},
            nColumns{dim.width}, nRows{dim.height} {}

    class PixelProxy
    {
    private:
        Image& img;
        Index index;

        PixelProxy(Image& ref, Index i) : img{ref}, index{i} {}

        friend class Image;
    public:
        void operator=(Pixel p)
        {
            img.redBuffer[index] = p.r;
            img.greenBuffer[index] = p.g;
            img.blueBuffer[index] = p.b;
        }

        operator Pixel () const
        {
            return {img.redBuffer[index],
                    img.greenBuffer[index],
                    img.blueBuffer[index]};
        }
    };

    /**
     * @brief Size in pixels of this image. Equivalent to `width * height`.
     * 
     * @return Number of pixels in this image
     */
    Index pixels() const { return blueBuffer.size(); }

    /**
     * @return This image dimensions: `{ width, height }`.
     */
    Dimensions dimensions() const { return {nColumns, nRows}; }

    Real luminance() const { return maxLuminance; }

    Natural resolution() const { return colorResolution;}

    void changeResolution(Natural c) { colorResolution = c;}

    PixelProxy operator()(Index i, Index j)
    {
        return {*this, i*nColumns + j};
    }

    Pixel operator()(Index i, Index j) const
    {
        return (*this)(i*nColumns + j);
    }

    PixelProxy operator()(Index i)
    {
        return {*this, i};
    }

    Pixel operator()(Index i) const
    {
        return {redBuffer[i], greenBuffer[i], blueBuffer[i]};
    }

    void toneMap(const ToneMappingStrategy& func);

    friend bool ppm::read(std::istream& is, Image& img);
    friend void ppm::write(std::ostream& os, const Image& img);
};

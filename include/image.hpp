#pragma once

#include <vector>

#include "numbers.hpp"
#include "tone_mapping.hpp"
#include "color_spaces.hpp"

#include "format/ppm.hpp"

using ErrorMsg = std::string;

class Image
{
private:
    std::vector<Real> redBuffer;
    std::vector<Real> greenBuffer;
    std::vector<Real> blueBuffer;
    
    Index nRows, nColumns;

public:
    Natural colorResolution;
    bool foundMaxValue;
    Real maxLuminance;

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

    PixelProxy operator()(Index i, Index j)
    {
        return {*this, i*nColumns + j};
    }

    Pixel operator()(Index i, Index j) const
    {
        Index index = i*nColumns + j;
        return {redBuffer[index], greenBuffer[index], blueBuffer[index]};
    }

    void map(const ToneMappingStrategy& f)
    {
        for (Index i = 0; i < redBuffer.size(); ++i)
        {
            Pixel p = {redBuffer[i], greenBuffer[i], blueBuffer[i]};
            auto [h, s, v] = HSVPixel::fromRGB(p, maxLuminance);
            maxLuminance = 1;
            auto [r, g, b] = HSVPixel::toRGB({h, s, f(v)}, maxLuminance);
            redBuffer[i] = r;
            greenBuffer[i] = g;
            blueBuffer[i] = b;
        }
    }

    friend bool ppm::read(std::istream& is, Image& img);
    friend void ppm::write(std::ostream& os, const Image& img);
};

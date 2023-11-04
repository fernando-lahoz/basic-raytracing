#pragma once

#include <vector>

#include "numbers.hpp"
#include "color_spaces.hpp"

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
    Image();

    Image(Real maxLum, Natural colorRes, Dimensions dim);

    class PixelProxy
    {
    private:
        Image& img;
        Index index;

        PixelProxy(Image& ref, Index i);

        friend class Image;
    public:
        void operator=(RGBPixel p);

        operator RGBPixel () const;
    };

    /**
     * @brief Size in pixels of this image. Equivalent to `width * height`.
     * 
     * @return Number of pixels in this image.
     */
    Index pixels() const;

    /**
     * @return This image dimensions: `{ width, height }`.
     */
    Dimensions dimensions() const;

    /**
     * @return Max value of luminance inside this image.
     */
    Real luminance() const;

    /**
     * @brief Updates the maximum luminance value within the image.
     * 
     * @return Max value of luminance inside this image after the update.
     */
    Real updateLuminance();

    Natural resolution() const;

    void changeResolution(Natural c);

    PixelProxy operator()(Index i, Index j);

    RGBPixel operator()(Index i, Index j) const;

    PixelProxy operator()(Index i);

    RGBPixel operator()(Index i) const;

    void toneMap(const ToneMappingStrategy& func);

    Real& red(Index i, Index j);

    Real red(Index i, Index j) const;

    Real& red(Index i);

    Real red(Index i) const;

    Real& green(Index i, Index j);

    Real green(Index i, Index j) const;

    Real& green(Index i);

    Real green(Index i) const;

    Real& blue(Index i, Index j);

    Real blue(Index i, Index j) const;

    Real& blue(Index i);

    Real blue(Index i) const;
};

#include "inline/image.ipp"

#pragma once

#include "image.hpp"

inline Image::Image()
    : Image{1, 255, {0, 0}}
{}

inline Image::Image(Real maxLum, Natural colorRes, Dimensions dim)
    : maxLuminance{maxLum}, colorResolution{colorRes},
        nColumns{dim.width}, nRows{dim.height}
{
    redBuffer.resize(dim.width * dim.height);
    greenBuffer.resize(dim.width * dim.height);
    blueBuffer.resize(dim.width * dim.height);
}

inline Index Image::pixels() const
{ 
    return blueBuffer.size();
}

inline Dimensions Image::dimensions() const 
{
    return {nColumns, nRows}; 
}

inline Real Image::updateLuminance() 
{
    Real max = 1;
    for (Index i : numbers::range(0, pixels()))
        max = numbers::max(red(i), green(i), blue(i), max);
    maxLuminance = max;
    return max;
}

inline Real Image::luminance() const 
{
    return maxLuminance;
}

inline Natural Image::resolution() const 
{
    return colorResolution;
}

inline void Image::changeResolution(Natural c)
{
    colorResolution = c;
}

inline Image::PixelProxy Image::operator()(Index i, Index j)
{
    return {*this, i*nColumns + j};
}

inline RGBPixel Image::operator()(Index i, Index j) const
{
    return (*this)(i*nColumns + j);
}

inline Image::PixelProxy Image::operator()(Index i)
{
    return {*this, i};
}

inline RGBPixel Image::operator()(Index i) const
{
    return {redBuffer[i], greenBuffer[i], blueBuffer[i]};
}

inline Real& Image::red(Index i, Index j)
{
    return redBuffer[i * nColumns + j];
}

inline Real Image::red(Index i, Index j) const
{
    return redBuffer[i * nColumns + j];
}

inline Real& Image::red(Index i)
{
    return redBuffer[i];
}

inline Real Image::red(Index i) const
{
    return redBuffer[i];
}

inline Real& Image::green(Index i, Index j)
{
    return greenBuffer[i * nColumns + j];
}

inline Real Image::green(Index i, Index j) const
{
    return greenBuffer[i * nColumns + j];
}

inline Real& Image::green(Index i)
{
    return greenBuffer[i];
}

inline Real Image::green(Index i) const
{
    return greenBuffer[i];
}

inline Real& Image::blue(Index i, Index j)
{
    return blueBuffer[i * nColumns + j];
}

inline Real Image::blue(Index i, Index j) const
{
    return blueBuffer[i * nColumns + j];
}

inline Real& Image::blue(Index i)
{
    return blueBuffer[i];
}

inline Real Image::blue(Index i) const
{
    return blueBuffer[i];
}

//---------------------------------------------------------------

inline Image::PixelProxy::PixelProxy(Image& ref, Index i)
    : img{ref}, index{i}
{}

inline void Image::PixelProxy::operator=(RGBPixel p)
{
    img.redBuffer[index] = p.r;
    img.greenBuffer[index] = p.g;
    img.blueBuffer[index] = p.b;
}

inline Image::PixelProxy::operator RGBPixel () const
{
    return {img.redBuffer[index],
            img.greenBuffer[index],
            img.blueBuffer[index]};
}

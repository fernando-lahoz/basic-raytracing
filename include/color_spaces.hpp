#pragma once

#include <cmath>
#include <tuple>

#include "numbers.hpp"

struct RGBTuple
{
    Real r, g, b;
};

struct RGBPixel : RGBTuple {};

struct HSVTuple
{
    Real h, s, v;
};

struct HSVPixel : HSVTuple
{
    static HSVPixel fromRGB(RGBPixel p);

    static RGBPixel toRGB(HSVPixel p);
};

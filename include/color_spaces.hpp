#pragma once

#include <cmath>
#include <tuple>

#include "numbers.hpp"

struct Pixel
{
    Real r, g, b;
};

struct HSVPixel
{
    Real h, s, v;

    static HSVPixel fromRGB(Pixel p, Real maxLuminance);

    static Pixel toRGB(HSVPixel p, Real maxLuminance);
};

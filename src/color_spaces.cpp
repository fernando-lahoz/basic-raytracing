#include "color_spaces.hpp"

#include <iostream>

HSVPixel HSVPixel::fromRGB(RGBPixel p)
{
    auto mod = [](Real x, Natural n) -> Real
    {   
        if (x < 0) return n - x;
        return x - n * static_cast<Natural>(x / n); 
    };

    auto [r, g, b] = p;
    Real max, min;
    max = numbers::max(r, g, b);
    min = numbers::min(r, g, b);

    Real diff = max - min;

    auto getH = [&]() -> Real
    {
        if (diff == 0)          return 0;
        else if (max == r)      return 60 * mod(((g - b) / diff), 6);
        else if (max == g)      return 60 * (((b - r) / diff) + 2);
        else /* max ==  b */    return 60 * (((r - g) / diff) + 4);
    };

    Real h = getH();
    Real s = max == 0 ? 0 : diff / max;
    Real v = max;

    return {h, s, v};
}

RGBPixel HSVPixel::toRGB(HSVPixel p)
{
    auto mod = [](Real x, Natural n) -> Real
        { return x - n * static_cast<Natural>(x / n); };

    const Real c = p.v * p.s;
    const Real x = c * (1 - std::abs(mod(p.h / 60, 2) - 1));
    const Real m = p.v - c;

    auto getRGB = [&]() -> RGBPixel
    {
        if (p.h < 60)         return {c, x, 0};
        else if (p.h < 120)   return {x, c, 0};
        else if (p.h < 180)   return {0, c, x};
        else if (p.h < 240)   return {0, x, c};
        else if (p.h < 300)   return {x, 0, c};
        else /* p.h < 360 */  return {c, 0, x};
    };

    auto [r, g, b] = getRGB();

    return {r + m, g + m, b + m};
}

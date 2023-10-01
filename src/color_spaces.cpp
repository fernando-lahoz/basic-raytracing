#include "color_spaces.hpp"

/*
 * https://www.rapidtables.com/convert/color/rgb-to-hsv.html
 */
HSVPixel HSVPixel::fromRGB(Pixel p, Real maxLuminance)
{
    Real r = p.r / maxLuminance;
    Real g = p.g / maxLuminance;
    Real b = p.b / maxLuminance;

    Real max, min;
    max = numbers::max(r, g, b);
    min = numbers::min(r, g, b);

    Real diff = max - min;
    auto getH = [&]() -> Real
    {
        if (diff == 0)          return 0;
        else if (max == r)      return 60 * numbers::mod(((g - b) / diff), 6);
        else if (max == g)      return 60 * (((b - r) / diff) + 2);
        else /* max ==  b */    return 60 * (((r - g) / diff) + 4);
    };

    Real h = getH();
    Real s = max == 0 ? 0 : diff / max;
    //Real v = max;
    Real v = max * maxLuminance;

    return {h, s, v};
}

/*
 * https://www.rapidtables.com/convert/color/hsv-to-rgb.html
 */
Pixel HSVPixel::toRGB(HSVPixel p, Real maxLuminance)
{
    const Real nv = p.v / maxLuminance;
    const Real c = nv * p.s;
    const Real x = c * (1 - std::abs(numbers::mod(p.h / 60, 2) - 1));
    const Real m = nv - c;

    auto getRGBnormalized = [&]()
    {
        if (p.h < 60)         return std::tuple{c, x, Real{0}};
        else if (p.h < 120)   return std::tuple{x, c, Real{0}};
        else if (p.h < 180)   return std::tuple{Real{0}, c, x};
        else if (p.h < 240)   return std::tuple{Real{0}, x, c};
        else if (p.h < 300)   return std::tuple{x, Real{0}, c};
        else /* p.h < 360 */  return std::tuple{c, Real{0}, x};
    };

    auto [r, g, b] = getRGBnormalized();

    auto denormalize = [&](Real x) -> Real
        { return std::round((x + m) * maxLuminance); };

    return {denormalize(r), denormalize(g), denormalize(b)};
}

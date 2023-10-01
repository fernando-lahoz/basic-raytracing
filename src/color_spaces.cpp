#include "color_spaces.hpp"

/*
 * https://www.rapidtables.com/convert/color/rgb-to-hsv.html
 */
HSVPixel HSVPixel::fromRGB(Pixel p, Real maxLuminance)
{
    auto mod = [](Real x, Natural n) -> Real
        { return x - n * static_cast<Natural>(x / n); };

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
        else if (max == r)      return 60 * mod(((g - b) / diff), 6);
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
    auto mod = [](Real x, Natural n) -> Real
        { return x - n * static_cast<Natural>(x / n); };

    const Real nv = p.v / maxLuminance;
    const Real c = nv * p.s;
    const Real x = c * (1 - std::abs(mod(p.h / 60, 2) - 1));
    const Real m = nv - c;

    auto getRGBnormalized = [&]() -> Pixel
    {
        if (p.h < 60)         return {c, x, 0};
        else if (p.h < 120)   return {x, c, 0};
        else if (p.h < 180)   return {0, c, x};
        else if (p.h < 240)   return {0, x, c};
        else if (p.h < 300)   return {x, 0, c};
        else /* p.h < 360 */  return {c, 0, x};
    };

    auto [r, g, b] = getRGBnormalized();

    auto denormalize = [&](Real x) -> Real
        { return (x + m) * maxLuminance; };

    return {denormalize(r), denormalize(g), denormalize(b)};
}

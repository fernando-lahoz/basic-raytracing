#include <iostream>
#include <cmath>
#include <tuple>

#include "numbers.hpp"

namespace rgb {

struct Pixel
{
    Natural r, g, b, c;
};

}

namespace hsv {

struct Pixel
{
    Real h, s, v;
};


Pixel fromRGB(rgb::Pixel p)
{
    auto normalize = [](Natural x, Natural c) -> Real
        { return x / static_cast<Real>(c); };

    Real r = normalize(p.r, p.c);
    Real g = normalize(p.g, p.c);
    Real b = normalize(p.b, p.c);

    Real max, min;
    max = std::max(std::max(r, g), b);
    min = std::min(std::min(r, g), b);

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

auto toRGB(Real h, Real s, Real v, Natural colorRes)
{
    const Real c = v * s;
    const Real x = c * (1 - std::abs(mod(h / 60, 2) - 1));
    const Real m = v - c;

    auto getRGBnormalized = [&]()
    {
        if (h < 60)         return std::tuple{c, x, Real{0}};
        else if (h < 120)   return std::tuple{x, c, Real{0}};
        else if (h < 180)   return std::tuple{Real{0}, c, x};
        else if (h < 240)   return std::tuple{Real{0}, x, c};
        else if (h < 300)   return std::tuple{x, Real{0}, c};
        else /* h < 360 */  return std::tuple{c, Real{0}, x};
    };

    auto [r, g, b] = getRGBnormalized();

    auto denormalize = [&](Real x) -> Natural
        { return std::round((x + m) * colorRes); };

    return std::tuple{denormalize(r), denormalize(g), denormalize(b)};
}

} //namespace hsv

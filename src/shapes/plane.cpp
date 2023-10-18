#include "shapes/plane.hpp"

Real Plane::intersect(Ray ray) const
{
    const auto [p, d] = ray;
    const auto nd = dot(n, d);
    if (nd == 0) return Ray::nohit;
    auto t = (dot(n, o - p)) / nd;
    return t <= 0 ? Ray::nohit : t;
}

Direction Plane::normal([[maybe_unused]] Point p) const
{
    return n;
}
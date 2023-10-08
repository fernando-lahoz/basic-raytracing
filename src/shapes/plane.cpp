#include "shapes/plane.hpp"

Real Plane::intersect(Ray ray) const
{
    const auto [p, d] = ray;
    const auto nd = dot(n, d);
    if (nd == 0) return Ray::nohit;
    return dot(n, o - p) / nd;
}

Direction Plane::normal([[maybe_unused]] Point p) const
{
    return n;
}
#include "shapes/plane.hpp"

Real Plane::intersect(Ray ray) const
{
    const auto [p, d] = ray;
    return dot(n, o - p) / dot(n, d);
}

Direction Plane::normal([[maybe_unused]] Point p) const
{
    return n;
}
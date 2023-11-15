#include "shapes/plane.hpp"
#include "ray_tracing.hpp"

Real Plane::intersect(const Ray& ray) const
{
    const auto [p, d] = ray;
    const auto nd = dot(n, d);
    if (nd == 0)
        return Ray::nohit;

    const auto t = (dot(n, o - p)) / nd;
    return t;
    // if (t < 0) return Ray::nohit;
    // else return t;
}

NormalReturn Plane::normal(const Direction d, const Point) const
{
    if (dot(n, d) <= 0)
        return {Side::out, n};
    else
        return {Side::in, -1 * n};
}

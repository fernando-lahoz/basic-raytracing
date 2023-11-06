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

Direction Plane::normal(const Direction d, const Point) const
{
    return -dot(n, d) * n;
}

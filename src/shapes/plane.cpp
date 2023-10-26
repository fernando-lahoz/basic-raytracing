#include "shapes/plane.hpp"
#include "ray_tracing.hpp"

Real Plane::intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const
{
    const auto [p, d] = ray;
    const auto nd = dot(n, d);
    if (nd == 0)
        return Ray::nohit;

    const auto t = (dot(n, o - p)) / nd;
    if (t < 0 || Ray::ge(t, minT))
        return Ray::nohit;

    hit = ray.hitPoint(t);
    normal = nd < 0 ? n : -1 * n;

    return t;
}

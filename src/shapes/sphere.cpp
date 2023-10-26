#include "shapes/sphere.hpp"

Real Sphere::intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const
{
    const auto [p, d] = ray;
    const auto p_c = p - c;
    const auto halfB = dot(d, p_c);
    const auto C = dot(p_c, p_c) - r * r;
    const auto delta = halfB * halfB - C;
    if (delta < 0)
        return Ray::nohit;
    
    const auto left = -halfB;
    const auto right = std::sqrt(delta);

    if (right <= left)
    {
        const auto t = left - right;
        if (Ray::ge(t, minT)) return Ray::nohit;

        const auto hitPoint = ray.hitPoint(t);
        hit = hitPoint;
        normal = (c - hitPoint) / r;
        return t;
    }
    else
    {
        const auto t = right;
        if (t >= minT) return Ray::nohit;

        const auto hitPoint = ray.hitPoint(t);
        hit = hitPoint;
        normal = (hitPoint - c) / r;
        return t;
    }
}

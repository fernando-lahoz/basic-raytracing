#include "shapes/sphere.hpp"

#include <iostream>

Real Sphere::intersect(const Ray& ray) const
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
    const auto t1 = left + right;
    const auto t2 = left - right;

    if (t1 < 0)
    {
        if (t2 < 0) return Ray::nohit;
        else return t2;
    }
    else
    {
        if (t2 < 0) return t1;
        else return numbers::min(t1, t2);
    }
}

Direction Sphere::normal(const Direction d, const Point hit) const
{
    const Direction n = c - hit;
    if (dot(n, d) > 0)
        return n / r;
    else
        return n / -r;
}

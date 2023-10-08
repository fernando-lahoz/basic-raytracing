#include "shapes/sphere.hpp"

Real Sphere::intersect(Ray ray) const
{

    const auto [p, d] = ray;
    const auto p_c = p - c;
    const auto halfB = dot(d, p_c);
    const auto C = dot(p_c, p_c) - r * r;
    const auto delta = halfB * halfB - C;
    if (delta < 0)
        return Ray::nohit;
    
    const auto left = -halfB;
    const auto right = sqrt(delta);

    // left > 0 & right > 0 -> left + right > 0
    // left - right < 0 === left < right -> left + right
    // else -> left - right
    return right <= left ? left - right : left + right;
}

Direction Sphere::normal(Point p) const
{
    return (p - c) / r;
}
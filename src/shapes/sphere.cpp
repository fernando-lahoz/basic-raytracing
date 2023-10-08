#include "shapes/sphere.hpp"

std::optional<Intersection> Sphere::intersect(Ray ray) const
{
    auto mulShift = [](Direction u, Direction v)
    {
        return Direction{u[0] * v[1], u[1] * v[2], u[2] * v[0]};
    };

    const auto [p, d] = ray;
    const auto p_c = p - c;
    const auto delta = r * r + 2 * dot(mulShift(d, d), mulShift(p_c, p_c));
    if (delta < 0)
        return std::nullopt;
    
    const auto left = -dot(d, p_c);
    const auto right = sqrt(delta);

    // left > 0 & right > 0 -> left + right > 0
    // left - right < 0 === left < right -> left + right
    // else -> left - right
    const auto t = left < right ? left - right : left + right;

    const auto hit = p + d * t;
    return Intersection{hit, (hit - c) / r};
}
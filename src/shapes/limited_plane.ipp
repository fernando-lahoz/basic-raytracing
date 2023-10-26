#pragma once

#include "shapes/plane.hpp"

template <typename BorderTy>
Real LimitedPlane<BorderTy>::intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const
{
    const auto [p, d] = ray;
    const auto nd = dot(n, d);
    if (nd == 0)
        return Ray::nohit;

    const auto t = (dot(n, o - p)) / nd;
    if (t < 0 || Ray::ge(t, minT))
        return Ray::nohit;

    const auto hitPoint = ray.hitPoint(t);
    if (border.isInside(hitPoint, *this))
        return Ray::nohit;

    hit = hitPoint;
    normal = nd < 0 ? -1 * n : n;

    return t;
}

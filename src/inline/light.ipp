#pragma once

#include "light.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"

template <typename ShapeTy>
bool AreaLight<ShapeTy>::isAreaLight() const
{
    return true;
}

template <typename ShapeTy>
Real AreaLight<ShapeTy>::intersect(const Ray& ray) const
{
    return shape.intersect(ray);
}

template <typename ShapeTy>
Direction AreaLight<ShapeTy>::normal(const Direction d, const Point hit) const
{
    return shape.normal(d, hit);
}

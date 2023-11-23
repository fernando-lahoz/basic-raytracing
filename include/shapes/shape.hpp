#pragma once

#include "shading.hpp"
#include "geometry.hpp"
#include "ray_tracing.hpp"

class Shape 
{
public:
    virtual Real intersect(const Ray& ray) const = 0;

    enum class Side {in, out};

    struct Normal
    {
        Side side;
        Direction normal;
    };

    virtual Normal normal(const Direction d, const Point hit) const = 0;
};
#pragma once

#include "color_spaces.hpp"
#include "ray_tracing.hpp"
#include "light.hpp"

class Shape 
{
protected:
    Color emission;

    Shape (Color color) : emission{color} {}
public:
    virtual Real intersect(const Ray& ray) const = 0;

    virtual Direction normal(const Direction d, const Point hit) const = 0;

    inline Color color() const { return emission; };
};
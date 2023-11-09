#pragma once

#include "shading.hpp"
#include "geometry.hpp"
#include "ray_tracing.hpp"

class Shape 
{
protected:
    Color color_;

    Shape (Color _color) : color_{_color} {}
public:
    virtual Real intersect(const Ray& ray) const = 0;

    virtual Direction normal(const Direction d, const Point hit) const = 0;

    inline Color color() const { return color_; }

    virtual bool isAreaLight() const;
};
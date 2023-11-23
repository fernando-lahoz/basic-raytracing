#pragma once

#include "shapes/shape.hpp"

class Sphere : public Shape
{
protected:
    Point c;
    Real r;
public:
    Sphere(const Point& center, Real radius)
        : c{center}, r{radius} {}

    virtual Real intersect(const Ray& ray) const override;

    virtual Normal normal(const Direction d, const Point hit) const override;
};

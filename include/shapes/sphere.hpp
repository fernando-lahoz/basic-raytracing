#pragma once

#include "shapes/shape.hpp"

class Sphere : public Shape
{
protected:
    Point c;
    Real r;
public:
    Sphere(Point center, Real radius, Emission color)
        : Shape{color}, c{center}, r{radius} {}

    virtual Real intersect(Ray ray) const override;

    virtual Direction normal(Point p) const override;
};

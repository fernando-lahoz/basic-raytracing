#pragma once

#include "shapes/shape.hpp"

class Plane : public Shape
{
protected:
    Point o;
    Direction n;
public:
    Plane(Point reference, Direction normal, Emission color)
        : Shape{color}, o{reference}, n{normalize(normal)} {}

    virtual Real intersect(Ray ray) const override;

    virtual Direction normal(Point p) const override;
};

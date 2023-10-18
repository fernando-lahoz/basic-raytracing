#pragma once

#include "shapes/shape.hpp"

class Plane : public Shape
{
protected:
    Direction n;
    Point o; 
public:
    Plane(Point reference, Direction normal, Emission color)
        : Shape{color}, n{normalize(normal)}, o{reference} {}

    virtual Real intersect(Ray ray) const override;

    virtual Direction normal(Point p) const override;
};

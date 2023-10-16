#pragma once

#include "shapes/shape.hpp"

class Plane : public Shape
{
protected:
    Direction n;
    Real k; // Distance to origin
public:
    Plane(Point reference, Direction normal, Emission color)
        : Shape{color}, n{normalize(normal)}, k{dot(reference, normal)} {}

    virtual Real intersect(Ray ray) const override;

    virtual Direction normal(Point p) const override;
};

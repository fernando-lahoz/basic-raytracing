#pragma once

#include "shapes/plane.hpp"

class Disk : public Plane
{
protected:
    Real r;
    Point o;
public:
    Disk(Direction normal, Point center, Real radius, Emission color)
        : Plane{center, normal, color}, r{radius}, o{center} {}

    virtual Real intersect(Ray ray) const override;
};

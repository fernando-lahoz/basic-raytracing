#pragma once

#include "shapes/plane.hpp"

class Disk : public Plane
{
protected:
    Real r;
public:
    Disk(Direction normal, Point center, Real radius, Emission color)
        : Plane{center, normal, color}, r{radius} {}

    virtual Real intersect(Ray ray) const override;

    virtual Direction normal(Point p) const override;
};

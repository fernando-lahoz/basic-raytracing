#pragma once

#include "color_spaces.hpp"
#include "raytracing.hpp"

struct Emission : RGBTuple {};

class Shape 
{
protected:
    Emission emission;

    Shape (Emission color) : emission{color} {}
public:
    virtual Real intersect(Ray ray) const = 0;

    virtual Direction normal(Point p) const = 0;

    inline Emission color() const { return emission; };
};
#pragma once

#include "geometry.hpp"
#include "color_spaces.hpp"

struct Ray
{
    Point p;
    Direction d; // must be normalized

    static inline constexpr Real nohit = -1;

    Point hitPoint(Real t) { return p + d * t; }
};

struct Emission : RGBTuple {};

class Shape 
{
protected:
    Emission emission;

    Shape (Emission color) : emission{color} {}
public:
    virtual Real intersect(Ray ray) const = 0;

    virtual Direction normal(Point p) const = 0;
};
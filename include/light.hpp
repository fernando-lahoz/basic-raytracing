#pragma once

#include "geometry.hpp"
#include "shapes.hpp"
#include "shading.hpp"

struct Ray;

class PointLight
{
private:
    Point p;
    Color e;
public:
    PointLight(Point point, Color emission)
        : p{point}, e{emission} {}

    inline Point position() const { return p; }

    inline Color color() const { return e; }
};

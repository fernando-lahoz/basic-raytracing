#pragma once

#include "geometry.hpp"
#include "shapes/plane.hpp"

#include <bitset>

struct PlainPoint { Real x, y; };

//If points are ordered contrary to selected direction,
//the enclosure will represent a silhouette (SCARAMOUCHE!!)
//instead of a solid polygon
enum class Closure { clockwise, counterclockwise };

//Concave friendly implementation -> TODO: specialize convex + regular polygon factory
template <Index N, Closure Cl = Closure::clockwise>
class Polygon : public Plane
{
    static_assert(N >= 3, "Polygons must have more than 3 vertices.");
private:
    Point vertices[N];

public:
    Polygon(const PlainPoint plain[N], Direction normal,
            Point origin, Point reference, Emission color);

    virtual Real intersect(Ray ray) const override;
};

#include "shapes/polygon.ipp"

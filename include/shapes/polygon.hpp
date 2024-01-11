#pragma once

#include "shapes/plane.hpp"

#include <vector>

struct FlatPoint
{
    Real x, y;
};

class PolygonBorder
{
protected:  
    std::vector<Point> vertices;
public:
    inline bool isInside(const Point p, const LimitedPlane<PolygonBorder>& plane) const;
    friend class Polygon;
};

CHECK_BORDER_CONCEPT(PolygonBorder)

/* Represents a convex polygon. */

class Polygon : public LimitedPlane<PolygonBorder>
{
public:
    inline Polygon(Direction normal, Point origin, Point reference,
            const std::vector<FlatPoint>& points, bool solid);
};

#include "shapes/polygon.ipp"
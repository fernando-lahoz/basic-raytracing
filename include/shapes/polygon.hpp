#pragma once

#include "shapes/plane.hpp"

#include <array>

struct FlatPoint
{
    Real x, y;
};

template <Index N>
class PolygonBorder
{
static_assert(N >= 3, "Polygon must have at least 3 vertices.");
protected:  
    std::array<Point, N> vertices;
public:
    inline bool isInside(const Point p, const LimitedPlane<PolygonBorder>& plane) const
    {
        //TODO
        return true;
    }
    
    friend LimitedPlane<PolygonBorder<N>>;
};

CHECK_BORDER_CONCEPT(PolygonBorder<3>)

/* Represents a convex polygon. */
template <Index N>
class Polygon : public LimitedPlane<PolygonBorder<N>>
{
public:
    Polygon(Direction normal, Point origin, Point reference,
            const std::array<FlatPoint, N>& points, Emission color)
        
        : LimitedPlane<PolygonBorder<N>>{origin, normal, PolygonBorder<N>{}, color}
    {
        const Direction k = Plane::n;
        const Direction j = normalize(cross(k, reference - origin));
        const Direction i = cross(j, k); // |j| = |k| = 1 -> |j x k| = 1
        Transformation planeToScene;
        planeToScene.revertBase({i, j, k, origin});

        for (Index v : numbers::range(0, N))
        {
            const auto[x, y] = points[v];
            this->border.vertices[v] = planeToScene * Point{x, y, 0};
        }
    }
};

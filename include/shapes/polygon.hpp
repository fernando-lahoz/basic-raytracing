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
    inline bool isInside(const Point p, const LimitedPlane<PolygonBorder>& plane) const
    {
        Index size = vertices.size();
        for (Index i : numbers::range(0, size))
        {
            const auto dir = cross(vertices[(i + 1) % size] - vertices[i], plane.n);
            if (dot(dir, (p - vertices[i])) < 0)
                return false;
        }
        return true;
    }
    
    friend class Polygon;
};

CHECK_BORDER_CONCEPT(PolygonBorder)

/* Represents a convex polygon. */

class Polygon : public LimitedPlane<PolygonBorder>
{
public:
    inline Polygon(Direction normal, Point origin, Point reference,
            const std::vector<FlatPoint>& points, bool solid)
        
        : LimitedPlane<PolygonBorder>{origin, normal, PolygonBorder{}, solid}
    {
        const Direction k = Plane::n;
        const Direction j = normalize(cross(k, reference - origin));
        const Direction i = cross(j, k); // |j| = |k| = 1 -> |j x k| = 1
        Transformation planeToScene;
        planeToScene.revertBase({i, j, k, origin});

        for (Index v : numbers::range(0, points.size()))
        {
            const auto[x, y] = points[v];
            this->border.vertices.emplace_back(planeToScene * Point{x, y, 0});
        }
        this->border.vertices.shrink_to_fit();
    }
};

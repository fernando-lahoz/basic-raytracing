#pragma once

#include "shapes/polygon.hpp"

bool PolygonBorder::isInside(const Point p, const LimitedPlane<PolygonBorder>& plane) const
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


Polygon::Polygon(Direction normal, Point origin, Point reference,
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
#pragma once

#include "shapes/polygon.hpp"

#include <iostream>

template <Index N, Closure Cl>
Polygon<N, Cl>::Polygon(const std::array<PlainPoint, N>& plain, Direction normal,
        Point origin, Point reference, Emission color)
    : Plane{origin, normal, color}
{
    const Direction k = Plane::n;
    const Direction j = normalize(cross(k, reference - origin));
    const Direction i = cross(j, k); // |j| = |k| = 1 -> |j x k| = 1
    Transformation planeToScene;
    planeToScene.revertBase({i, j, k, origin});

    for (Index v : std::views::iota(Index{0}, N))
    {
        const auto[x, y] = plain[v];
        vertices[v] = planeToScene * Point{x, y, 0};
    }
}

template <Index N, Closure Cl>
Real Polygon<N, Cl>::intersect(Ray ray) const 
{
    const auto t = Plane::intersect(ray);
    if (t == Ray::nohit)
        return Ray::nohit;

    const auto hit = ray.hitPoint(t);
    auto isInside = [&](Index v1, Index v2) -> bool
    {
        Direction u;
        if constexpr (Cl == Closure::clockwise) 
            { u = cross(vertices[v2 % N] - vertices[v1 % N], Plane::n); }
        else
            { u = cross(vertices[v1 % N] - vertices[v2 % N], Plane::n); }
        return dot(hit - vertices[v1 % N], u) >= 0;
    };

    std::bitset<N> checked, result;
    auto checkSegment = [&](Index vertex) -> bool
    {
        Index a = vertex % N;
        if (checked[a]) return result[a];
        bool ok = isInside(vertex, vertex + 1);
        checked[a] = true; result[a] = ok;
        return ok;
    };

    std::bitset<N> blacklist, whitelist;
    for (Index v : std::views::iota(Index{0}, N))
    {
        // if (blacklist.test(v))
        //     continue;

        if (!checkSegment(v))
        {
            // dedicated functions not used due to exceptions overhead
            blacklist[v] = true; 
            blacklist[(v + 1) % N] = true;
        }
        else if (!checkSegment(v + 1))
        {
            blacklist[(v + 1) % N] = true; 
            blacklist[(v + 2) % N] = true;
        }
        else if (isInside(v + 2, v))
        {
            whitelist[v] = true; 
        }
    }

    auto final = whitelist &~ blacklist; //whitelist - blacklist
    return final.any() ? t : Ray::nohit;
}
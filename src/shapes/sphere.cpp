#include "shapes/sphere.hpp"

#include <iostream>

Real Sphere::intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const
{
    const auto [p, d] = ray;
    const auto p_c = p - c;
    const auto halfB = dot(d, p_c);
    const auto C = dot(p_c, p_c) - r * r;
    const auto delta = halfB * halfB - C;
    if (delta < 0)
        return Ray::nohit;
    
    const auto left = -halfB;
    const auto right = std::sqrt(delta);
    const auto t1 = left + right;
    const auto t2 = left - right;

    const auto t = [&]() 
    {
        if (t1 < 0)
        {
            if (t2 < 0) return Ray::nohit;
            else return t2;
        }
        else
        {
            if (t2 < 0) return t1;
            else return numbers::min(t1, t2);
        }
    }();

    if (Ray::further(t, minT)) return Ray::nohit;

    if (left > 0)
    { // Ray from OUTSIDE
        const auto hitPoint = ray.hitPoint(t);
        hit = hitPoint;
        normal = (hitPoint - c) / r;

        //std::cout << "Ray origin: " << p << "; Normal: " << normal << "; Hitpoint: " << hitPoint << '\n';
        return t;
    }
    else
    { // Ray from INSIDE
        const auto hitPoint = ray.hitPoint(t);
        hit = hitPoint;
        normal = (c - hitPoint) / r;

        //std::cout << "Ray origin: " << p << "; Normal: " << normal << "; Hitpoint: " << hitPoint << '\n';
        return t;
    }
}

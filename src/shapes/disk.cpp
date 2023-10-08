#include "shapes/disk.hpp"
#include "shapes/plane.hpp"

Real Disk::intersect(Ray ray) const
{
    const auto t = Plane::intersect(ray);
    if (t == Ray::nohit)
        return Ray::nohit;

    const auto hit = ray.hitPoint(t);
    if (norm(hit - o) > r)
        return Ray::nohit;

    return t;
}

Direction Disk::normal([[maybe_unused]] Point p) const
{
    return n;
}
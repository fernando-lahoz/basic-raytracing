#include "shape.hpp"

class Sphere : Shape
{
private:
    Point c;
    Real r;
public:
    Sphere(Point center, Real radius)
        : c{center}, r{radius} {}

    virtual IntersectResult intersect(Ray ray) const override
    {
        const auto [p, d] = ray;
        const auto dR = norm(d * r);
        if (dR > r)
            return {.intersects = false};

        const auto proj = dot(d, (p - c));
        const auto t = proj > dR ? proj - dR : proj + dR;
        const auto hit = p + d * t;
        return {hit, normalize(hit - c), true};
    }
};

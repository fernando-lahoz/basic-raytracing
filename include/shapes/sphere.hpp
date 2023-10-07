#include "shape.hpp"

class Sphere : Shape
{
private:
    Point c;
    Real r;
public:
    Sphere(Point center, Real radius) : c{center}, r{radius} {}

    virtual IntersectResult intersect(Ray ray) const override
    {
        const auto [p, d] = ray;
        const auto d2 = dot(d, d);
        const auto c2_r2 = dot(c, c) - r * r;
        const auto delta = d2 * c2_r2;
        if (delta < 0)
            return {.check =  std::numeric_limits<Real>::infinity()};

        const auto c_p = c - p;
        const auto dc_p = dot(d, c_p);
        const auto sqrt_delta = std::sqrt(delta);
        const auto t_1 = (dc_p + sqrt_delta) / d2;
        const auto t_2 = (dc_p - sqrt_delta) / d2;

        if (t_1 >= 0 && t_1 < t_2)
            return {.intersection = p + d * t_1};
        
        return {.intersection = p + d * t_2};
    }
};

#include "shapes/shape.hpp"

class Sphere : Shape
{
private:
    Point c;
    Real r;
public:
    Sphere(Point center, Real radius)
        : c{center}, r{radius} {}

    virtual std::optional<Intersection> intersect(Ray ray) const override;
};

#pragma once

#include "shapes/shape.hpp"

class Sphere : public Shape
{
protected:
    Point c;
    Real r;
public:
    Sphere(Point center, Real radius, Color color)
        : Shape{color}, c{center}, r{radius} {}

    virtual Real intersect(const Ray& ray) const override;

    virtual Direction normal(const Direction d, const Point hit) const override;
    
    friend std::ostream& operator<<(std::ostream& os, const Sphere& sphere);
};

inline std::ostream& operator<<(std::ostream& os, const Sphere& sphere)
{
    os << "Sphere {"             << '\n';
    os << "    Center: " << sphere.c << '\n';
    os << "    Radius: " << sphere.r << '\n';
    os << "}"                    << '\n';
    return os;
}
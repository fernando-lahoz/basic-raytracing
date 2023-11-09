#pragma once

#include "geometry.hpp"
#include "shapes.hpp"
#include "shading.hpp"

struct Ray;

class PointLight
{
private:
    Point p;
    Color e;
public:
    PointLight(Point point, Color emission)
        : p{point}, e{emission} {}

    inline Point position() const { return p; }

    inline Color color() const { return e; }
};

template <typename ShapeTy>
class AreaLight : public Shape
{
private:
    ShapeTy shape;
public:
    AreaLight(const ShapeTy& shape_)
        : Shape{shape_.color()}, shape{shape_} {}

    virtual bool isAreaLight() const override;

    virtual Real intersect(const Ray& ray) const override;

    virtual Direction normal(const Direction d, const Point hit) const override;
};

#include "inline/light.ipp"
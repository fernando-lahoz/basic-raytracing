#pragma once

#include <concepts>

#include "shapes/shape.hpp"

template <typename Ty, template<typename...> typename PlaneTy>
concept BorderClass = requires (const Ty border, const Point p, const PlaneTy<Ty>& base)
{
    {border.isInside(p, base)} -> std::same_as<bool>;
};

template <typename BorderTy>
class LimitedPlane : public Shape
{
protected:
    Direction n;
    Point o;
    BorderTy border;
public:
    LimitedPlane(Point reference, Direction normal, BorderTy border, Emission color)
        : Shape{color}, n{normalize(normal)}, o{reference}, border{border} {}

    virtual Real intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const override;

    friend BorderTy;
};

class Plane : public Shape
{
protected:
    Direction n;
    Point o;
public:
    Plane(Point reference, Direction normal, Emission color)
        : Shape{color}, n{normalize(normal)}, o{reference} {}

    virtual Real intersect(const Ray& ray, Real minT, Point& hit, Direction& normal) const override;
};

#define CHECK_BORDER_CONCEPT(BorderTy) static_assert(BorderClass<BorderTy, LimitedPlane>);

#include "shapes/plane.ipp"
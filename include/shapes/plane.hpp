#pragma once

#include <concepts>

#include "shapes/shape.hpp"

template <typename Ty, template<typename...> typename PlaneTy>
concept BorderClass = requires (const Ty border, const Point p, const PlaneTy<Ty>& base)
{
    {border.isInside(p, base)} -> std::same_as<bool>;
};

class Plane : public Shape
{
protected:
    Direction n;
    Point o;
public:
    Plane(const Point& reference, const Direction& normal)
        : n{normalize(normal)}, o{reference} {}

    virtual Real intersect(const Ray& ray) const override;

    virtual Normal normal(const Direction d, const Point hit) const override;
};

template <typename BorderTy>
class LimitedPlane : public Plane
{
protected:
    BorderTy border;
public:
    LimitedPlane(Point reference, Direction normal, BorderTy border)
        : Plane{reference, normal}, border{border} {}

    virtual Real intersect(const Ray& ray) const override
    {
        const auto t = Plane::intersect(ray);
        if (!border.isInside(ray.hitPoint(t), *this))
            return t;
        return Ray::nohit;
    }

    friend BorderTy;
};

#define CHECK_BORDER_CONCEPT(BorderTy) static_assert(BorderClass<BorderTy, LimitedPlane>);

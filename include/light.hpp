#pragma once

#include "geometry.hpp"

class Color
{
private:
    Real r, g, b;
public:
    inline Color(Real red, Real green, Real blue)
        : r{red}, g{green}, b{blue} {}

    inline Color operator+(const Color& other) const
    {
        return {this->r + other.r, this->g + other.g, this->b + other.b};
    }

    inline Color operator*(const Color& other) const
    {
        return {this->r * other.r, this->g * other.g, this->b * other.b};
    }

    inline Color operator/(const Real k) const
    {
        return {r / k, g / k, b / k};
    }

    inline Color operator*(const Real k) const
    {
        return {r * k, g * k, b * k};
    }

    operator RGBPixel() const
    {
        return {r, g, b};
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        return os << "(" << color.r << ", " << color.g << ", " << color.b << ")";
    }
};

class PointLight
{
private:
    Point p;
    Color e;
public:
    PointLight(Point point, Color emission)
        : p{point}, e{emission} {}

    inline Point position() const
    {
        return p;
    }

    inline Color color() const
    {
        return e;
    }
};

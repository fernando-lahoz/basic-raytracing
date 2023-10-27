#pragma once

#include "geometry.hpp"

class Emission
{
private:
    Real r, g, b;
public:
    inline Emission(Real red, Real green, Real blue)
        : r{red}, g{green}, b{blue} {}

    inline Emission operator+(const Emission& other) const
    {
        return {this->r + other.r, this->g + other.g, this->b + other.b};
    }

    inline Emission operator/(const Real k) const
    {
        return {r / k, g / k, b / k};
    }

    operator RGBPixel() const
    {
        return {r, g, b};
    }
};

class PointLight
{
private:
    Point p;
    Emission e;
public:
    PointLight(Point point, Emission emission)
        : p{point}, e{emission} {}

    inline Point position() const
    {
        return p;
    }
};

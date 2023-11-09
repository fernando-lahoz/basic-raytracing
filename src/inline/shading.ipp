#pragma once

#include "shading.hpp"

inline Color::Color(Real red, Real green, Real blue)
    : r{red}, g{green}, b{blue}
{}

inline Color Color::operator+(const Color& other) const
{
    return {this->r + other.r, this->g + other.g, this->b + other.b};
}

inline Color Color::operator*(const Color& other) const
{
    return {this->r * other.r, this->g * other.g, this->b * other.b};
}

inline Color operator*(const Color& color, const Real k)
{
    return {color.r * k, color.g * k, color.b * k};
}

inline Color operator*(const Real k, const Color& color)
{
    return color * k;
}

inline Color operator/(const Color& color, const Real k)
{
    return {color.r / k, color.g / k, color.b / k};
}

inline Color::operator RGBPixel() const
{
    return {r, g, b};
}

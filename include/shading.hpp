#pragma once

#include "image.hpp"

class Color
{
private:
    Real r, g, b;
public:
    Color();
    Color(Real red, Real green, Real blue);

    inline Real p() const { return numbers::max(r, g, b); }

    Color operator+(const Color& other) const;
    Color operator*(const Color& other) const;
    friend Color operator*(const Color& color, const Real k);
    friend Color operator*(const Real k, const Color& color);
    friend Color operator/(const Color& color, const Real k);
    
    operator RGBPixel() const;

    friend std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        return os << "(" << color.r << ", " << color.g << ", " << color.b << ")";
    }
};

#include "inline/shading.ipp"
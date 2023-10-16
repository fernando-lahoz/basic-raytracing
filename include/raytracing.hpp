#pragma once

#include "geometry.hpp"
#include "image.hpp"

#include <vector>
#include <ostream>

class Shape;

struct Ray
{
    Point p;
    Direction d; // must be normalized

    static inline constexpr Real nohit = -1;

    inline Point hitPoint(Real t) { return p + d * t; }
};

class Camera
{
private:
    Direction f, l, u;
    Point o;
public:
    Camera(Point pinhole, Direction front, Direction up);

    void render(Image& img, const std::vector<std::reference_wrapper<const Shape>>&  objects) const;

    friend std::ostream& operator<<(std::ostream& os, const Camera& cam);
};

inline std::ostream& operator<<(std::ostream& os, const Camera& cam)
{
    os << "Camera {"             << '\n';
    os << "    Front: " << cam.f << '\n';
    os << "    Left: "  << cam.l << '\n';
    os << "    Up: "    << cam.u << '\n';
    os << "    Focus: " << cam.o << '\n';
    os << "}"                    << '\n';
    return os;
}
#include <iostream>

#include "shapes/sphere.hpp"

int main()
{
    const Real radius = 1;
    const Point center {1, 0, 0};

    Sphere sphere {center, radius};

    Ray ray {.p = {0, 0, 0}, .d = {1, 0, 0}};

    std::cout << "Sphere{.radius = " << radius << ", .center = " << center << "}\n";

    auto intersects = sphere.intersect(ray);
    if (!intersects)
    {
        std::cout << "Does not intersect\n";
    }
    else
    {
        auto [intersection, normal] = intersects.value();
        std::cout << "Intersects in point " << intersection << " with normal vector " << normal << "\n";
    }

    return 0;
}
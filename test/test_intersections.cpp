#include <iostream>

#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"

void testIntersection(const Shape& object, Ray ray)
{
    auto t = object.intersect(ray);
    if (t != Ray::nohit)
    {
        auto intersection = ray.hitPoint(t);
        auto normal = object.normal(intersection);
        std::cout << "Intersects in point " << intersection << " with normal vector " << normal << "\n";
        std::cout << "|n| = " << norm(normal) << '\n';
        return;
    }

    std::cout << "Does not intersect\n";
}

int main()
{
    
    Ray ray {.p = {0, 0, 0}, .d = normalize({1, 0, 0})};

    const Real radius = 1;
    const Point center {1, 1, 0};

    std::cout << "Sphere{.radius = " << radius << ", .center = " << center << "}\n";
    testIntersection(Sphere{center, radius, {255, 0, 255}}, ray);
    std::cout << std::endl;

    //------------------------------------------------------------------------------

    const Direction normal = {-1, 1, 0};
    const Point reference {1, 1, 0};

    std::cout << "Plane{.normal = " << normal << ", .reference = " << reference << "}\n";
    testIntersection(Plane{reference, normal, {255, 0, 255}}, ray);
    std::cout << std::endl;

    return 0;
}
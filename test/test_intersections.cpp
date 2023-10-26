#include <iostream>

#include "shapes.hpp"
#include "ray_tracing.hpp"

void testReal(const Shape& object, Ray ray)
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
    {
        Ray ray {.p = {0, 0, 0}, .d = normalize({1, 0, 0})};

        const Real radius = 1;
        const Point center {1, 1, 0};

        std::cout << "Ray{.origin = " << ray.p << ", .direction = " << ray.d << "}\n";
        std::cout << "Sphere{.radius = " << radius << ", .center = " << center << "}\n";
        testReal(Sphere{center, radius, {255, 0, 255}}, ray);
        std::cout << std::endl;
    }
    
    //------------------------------------------------------------------------------

    {
        Ray ray {.p = {0, 0, 0}, .d = normalize({1, 0, 0})};

        const Direction normal = {-1, 1, 0};
        const Point reference {1, 1, 0};

        std::cout << "Ray{.origin = " << ray.p << ", .direction = " << ray.d << "}\n";
        std::cout << "Plane{.normal = " << normal << ", .reference = " << reference << "}\n";
        testReal(Plane{reference, normal, {255, 0, 255}}, ray);
        std::cout << std::endl;
    }
    
    //------------------------------------------------------------------------------

    {
        Ray ray {.p = {0, 0, 0}, .d = normalize({1, 0.707108, 0.707108})};

        const Direction normal = {-1, 0, 0};
        const Real radius = 1;
        const Point center {1, 0, 0};

        std::cout << "Ray{.origin = " << ray.p << ", .direction = " << ray.d << "}\n";
        std::cout << "Disk{.normal = " << normal << ", .radius = " << radius << ", .center = " << center << "}\n";
        testReal(Disk(normal, center, radius, {255, 0, 255}), ray);
        std::cout << std::endl;
    }

    {
        Ray ray {.p = {0, 0, 0}, .d = normalize({1, 0, 0})};
        // NOT TESTED
        const PlainPoint points[3] = {{0,0}, {0,0}, {0,0}};
        Polygon<3> triangle {points, {1, 1, 1}, {1, 0, 0}, {1, 1, 0}, {0, 0, 0}};
        testReal(triangle, ray);
        std::cout << std::endl;
    }
}
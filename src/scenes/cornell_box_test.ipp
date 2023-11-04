#pragma once

#include "shapes.hpp"
#include "ray_tracing.hpp"

#include <iostream>
#include <array>

namespace cornell_box_test
{

namespace cam
{
    Point focus {0, 0, -3.5};
    Direction front {0, 0, 3};
    Direction up {0, 1, 0};
}

Sphere left_sphere {Point{-0.5, -0.7, 0.25}, 0.3, Emission{0, 1, 1}};
Sphere right_sphere {Point{0.5, -0.7, -0.25}, 0.3, Emission{1, 0, 1}};

Plane right_plane {Point{-1, 0, 0}, Direction{1, 0, 0}, Emission{0, 1, 0}};
Plane left_plane {Point{1, 0, 0}, Direction{1, 0, 0}, Emission{1, 0, 0}};
Plane floor_plane {Point{0, 1, 0}, Direction{0, 1, 0}, Emission{1, 1, 1}};
Plane ceiling_plane {Point{0, -1, 0}, Direction{0, 1, 0}, Emission{1, 1, 1}};
Plane back_plane {Point{0, 0, -1}, Direction{0, 0, 1}, Emission{1, 1, 1}};

PointLight light {{0, 0.5, 0}, {1, 1, 1}};

ObjectSet objects
{
    {
        left_sphere, right_sphere,
        right_plane,
        left_plane,
        floor_plane,
        ceiling_plane,
        back_plane
    },
    {
        light
    }
};

class Init
{
public:
    Init()
    {
        // objects.push_back(std::cref(blue_ball_1));
        // objects.push_back
        // std::cout << "Rendering..." << std::endl;
        // std::cout << camera << std::endl;
        // std::cout << (const Sphere&) objects[0].get() << std::endl;
    }
};

Init init {};

} //namespace cornell_box_1


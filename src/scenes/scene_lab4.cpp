#pragma once

#include "shapes.hpp"
#include "ray_tracing.hpp"

#include <iostream>
#include <array>

namespace cornell_box_1
{

namespace cam
{
    Point focus {0, 0, -3.5};
    Direction front {2, 0, 0};
    Direction up {0, 1, 0};
}

Sphere ball_1 {Point{6, 0, 0}, 1, Emission{1, 0, 0}};
Sphere ball_2 {Point{7, -1.2, 0.8}, 0.9, Emission{0, 1, 1}};

Plane wall_background {Point{10, 0, 0}, Direction{-1, 0, 0}, Emission{1, 1, 1}};
Plane wall_left {Point{0, 0, -3}, Direction{0, 0, 1}, Emission{1, 1, 0}};
Plane wall_right {Point{0, 0, 3}, Direction{0, 0, -1}, Emission{1, 0, 1}};
Plane wall_roof {Point{0, 3, 0}, Direction{0, -1, 0}, Emission{0, 1, 0}};
Plane wall_floor {Point{0, -3, 0}, Direction{0, 1, 0}, Emission{0, 0, 1}};

ObjectSet objects
{
    ball_1, ball_2,
    wall_background, wall_roof, wall_floor,
    wall_left, wall_right
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


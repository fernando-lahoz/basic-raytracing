#pragma once

#include "object_set.hpp"
#include "ray_tracing.hpp"

#include <iostream>
#include <array>

namespace cornell_box_1
{

namespace cam
{
    Point focus {0, 0, 0};
    Direction front {2, 0, 0};
    Direction up {0, 1, 0};
}

Sphere ball_1 {Point{6, 0, 0}, 1, Color{1, 0, 0}};
Sphere ball_2 {Point{7, -1.2, 0.8}, 0.9, Color{0, 1, 1}};
Sphere ball_3 {Point{5, 0.5, -1.1}, 0.5, Color{0.6, 0.2, 0.6}};

Plane wall_background {Point{10, 0, 0}, Direction{-1, 0, 0}, Color{1, 1, 1}};
Plane wall_left {Point{0, 0, -3}, Direction{0, 0, 1}, Color{1, 1, 0}};
Plane wall_right {Point{0, 0, 3}, Direction{0, 0, -1}, Color{1, 0, 1}};
Plane wall_roof {Point{0, 3, 0}, Direction{0, -1, 0}, Color{0, 1, 0}};
Plane wall_floor {Point{0, -3, 0}, Direction{0, 1, 0}, Color{0, 0, 1}};

//Disk disk_1 {{-0.3, -0.5, 0.2}, {4.5, -1.2, 0}, 1, {0.01, 0.01, 0.02}};

PointLight light {{4.5, 2.6, -1}, {1, 1, 1}};
PointLight light2 {{5.5, -2.6, 2}, {1, 1, 1}};

ObjectSet objects
{
    { // objects
        ball_1, ball_2, ball_3,
        wall_background, wall_roof, wall_floor,
        wall_left, wall_right,
        //disk_1
    },
    { // lights
        light, light2
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


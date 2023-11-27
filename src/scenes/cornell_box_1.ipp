#pragma once

#include "object_set.hpp"
#include "ray_tracing.hpp"

#define MS(Ty) std::make_shared<Ty>

namespace cornell_box_1
{

namespace cam
{
    Point focus {0, 0, 0};
    Direction front {2, 0, 0};
    Direction up {0, 1, 0};
}


Object ball_1 {
    MS(Sphere)(Point{6, 0, 0}, 1),
    MS(Material)(diffuse({0.9, 0, 0}))
};

Object ball_2 {
    MS(Sphere)(Point{7, -1.2, 0.8}, 0.9),
    MS(Material)(diffuse({0, 0.9, 0.9}))
};

Object ball_3 {
    MS(Sphere)(Point{5, 0.5, -1.1}, 0.5),
    MS(Material)(diffuse({0.6, 0.2, 0.6}))
};


Object wall_background {
    MS(Plane)(Point{10, 0, 0},  Direction{-1, 0, 0}),
    MS(Material)(diffuse({0.9, 0.9, 0.9}))
};

Object wall_left {
    MS(Plane)(Point{0, 0, -3},  Direction{0, 0, 1}),
    MS(Material)(diffuse({0.9, 0.9, 0}))
};

Object wall_right {
    MS(Plane)(Point{0, 0, 3},  Direction{0, 0, -1}),
    MS(Material)(diffuse({0.9, 0, 0.9}))
};

Object wall_floor {
    MS(Plane)(Point{0, -3, 0},  Direction{0, 1, 0}),
    MS(Material)(diffuse({0, 0, 0.9}))
};

Object wall_roof {
    MS(Plane)(Point{0, 3, 0},  Direction{0, -1, 0}),
    MS(Material)(diffuse({0, 0.9, 0}))
};

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

    }
};

Init init {};

} //namespace cornell_box_1


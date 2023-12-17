#pragma once

#include "object_set.hpp"
#include "ray_tracing.hpp"

#define MS(Ty) std::make_shared<Ty>

namespace cornell_box_test
{

namespace cam
{
    Point focus {0, 0, -3.5};
    Direction front {0, 0, 3};
    Direction up {0, 1, 0};
}

Object left_sphere {
    MS(Sphere)(Point{0.5, -0.7, 0.25}, 0.3),
    MS(Material)(diffuse({0.3, 0.6, 0.6}) + specular({0.3, 0.3, 0.3}))
    //MS(Material)(diffuse({0.7, 0.575, 0.8}))
};

Object right_sphere {
    MS(Sphere)(Point{-0.5, -0.7, -0.25}, 0.3),
    MS(Material)(specular({0.25, 0.25, 0.25}) + refractive({0.7, 0.7, 0.7}, 1.5))
    //MS(Material)(refractive({0.95, 0.95, 0.95}, 1.5))
    //MS(Material)(diffuse({0.5, 0.9, 0.9}))
};


Object left_plane {
    MS(Plane)(Point{1, 0, 0},  Direction{-1, 0, 0}),
    MS(Material)(diffuse({0.8, 0, 0}))
};

Object right_plane {
    MS(Plane)(Point{-1, 0, 0}, Direction{1, 0, 0}),
    MS(Material)(diffuse({0, 0.8, 0}))
};

Object floor_plane {
    MS(Plane)(Point{0, -1, 0}, Direction{0, 1, 0}),
    MS(Material)(diffuse({0.9, 0.9, 0.9}))
};

Object back_plane {
    MS(Plane)(Point{0, 0, 1},  Direction{0, 0, -1}),
    MS(Material)(diffuse({0.9, 0.9, 0.9}))
};

Object ceiling_plane {
    MS(Plane)(Point{0, 1, 0},  Direction{0, -1, 0}),
    //MS(Material)(emitter({0.9, 0.9, 0.9}))
    MS(Material)(diffuse({0.9, 0.9, 0.9}))
};

PointLight light {{0, 0.5, 0}, {1, 1, 1}};

ObjectSet objects
{
    {
        left_sphere, right_sphere,
        right_plane, left_plane,
        floor_plane, ceiling_plane,
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
        
    }
};

Init init {};

} //namespace cornell_box_test


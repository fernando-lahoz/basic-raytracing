#pragma once

#include "geometry.hpp"
#include "image.hpp"
#include "random.hpp"
#include "shading.hpp"

struct Ray
{
    Point p;
    Direction d; // must be normalized

    constexpr static inline Real nohit = -1;

    static inline bool isHit(Real t) { return t >= 0; }

    inline Point hitPoint(Real t) const { return p + (d * t); }
};

class Camera
{
private:
    Direction f, l, u;
    Point o;
    PrecisionReal pixelWidth, pixelHeight; //precision-independent

    UniformDistribution randomX;
    UniformDistribution randomY;
    RandomSeed rd;
    RandomGenerator gen;
public:
    Camera(Point pinhole, Direction front, Direction up, Dimensions dim);
    Camera(const Camera& cam);

    Ray randomRay(Index i, Index j);
};


class Object;
struct ObjectSet;

struct Intersection
{
    Real distance;
    const Object* hitObject;

    inline operator bool() { return distance >= 0; } 
};

Intersection findIntersection(const ObjectSet& objSet, const Ray& ray);

Color castShadowRays(const ObjectSet& objSet, const Direction& normal,
        const Point& hit, const Color& kd);

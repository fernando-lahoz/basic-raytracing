#pragma once

#include "geometry.hpp"
#include "image.hpp"
#include "light.hpp"

#include <vector>
#include <ostream>
#include <random>

class Shape;

#if 0
using ObjectSet = std::vector<std::reference_wrapper<const Shape>>;
#endif

struct ObjectSet
{
    std::vector<std::reference_wrapper<const Shape>> objects;
    std::vector<std::reference_wrapper<const PointLight>> pointLights;
};


struct Ray
{
    Point p;
    Direction d; // must be normalized

    constexpr static inline Real nohit = -1;

    static inline bool further(Real t, Real minT) { return minT >= 0 && t >= minT; }

    inline Point hitPoint(Real t) const { return p + (d * t); }
};

using UniformDistribution = std::uniform_real_distribution<PrecisionReal>;
using RandomSeed = std::random_device;
using RandomGenerator = std::mt19937;

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


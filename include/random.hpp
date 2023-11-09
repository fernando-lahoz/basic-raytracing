#pragma once

#include "numbers.hpp"

#include <random>

using UniformDistribution = std::uniform_real_distribution<PrecisionReal>;
using RandomSeed = std::random_device;
using RandomGenerator = std::mt19937;

class Randomizer
{
private:
    RandomSeed seed;
    RandomGenerator gen;
    UniformDistribution dist;
public:
    inline Randomizer(Real s = 0, Real f = 1) : seed{}, gen{seed()}, dist{s, f} {}
    
    inline Real operator()() { return dist(gen); }
};
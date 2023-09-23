#pragma once 

#include <cassert>
#include <tuple>
#include <optional>
#include <cmath>

#include "geometry.hpp"

struct PlanetParameterPack { Point center; Point refCity; Direction axis; };

std::optional<PlanetParameterPack>
makePlanetParameterPack(Point center, Point refCity, Direction axis);

class Planet
{
private:
    Point center;
    Point refCity;
    Direction axis;
    Base local;

public:
    Planet(PlanetParameterPack pack);

    const Base& getLocalBase() { return local; }

    friend class PlanetaryStation;
};

class PlanetaryStation 
{
private:
    Point position;
    Base local;

public:
    PlanetaryStation(const Planet& planet, Real inclination, Real azimuth);

    Point getPosition() { return position; }

    const Base& getLocalBase() { return local; }
};
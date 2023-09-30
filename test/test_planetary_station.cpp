#include <iostream>
#include <tuple>
#include <cassert>
#include <tuple>
#include <optional>
#include <cmath>

#include "geometry.hpp"

#define ENDL std::cout << '\n';

struct PlanetParameterPack { Point center; Point refCity; Direction axis; };

std::optional<PlanetParameterPack>
makePlanetParameterPack(Point center, Point refCity, Direction axis)
{
    Real radius = norm(refCity - center);
    Real axisHalf = norm(axis) / 2.0f;

    if (std::abs(axisHalf - radius) > 1E-6)
        return {std::nullopt};

    return PlanetParameterPack{center, refCity, axis};
}

class Planet
{
private:
    Point center;
    Point refCity;
    Direction axis;
    Base local;

public:
    Planet(PlanetParameterPack pack)
        : center(pack.center), refCity(pack.refCity), axis(pack.axis)
    {
        Direction i, j, k;
        Direction refRadius = refCity - center;
        Real radius = norm(refRadius);

        j = axis / 2.0f;
        k = normalize(cross(refRadius, j)) * radius;
        i = cross(j, k) / radius;

        local = Base{i, j, k, center};
    }

    const Base& getLocalBase() { return local; }

    friend class PlanetaryStation;
};

class PlanetaryStation 
{
private:
    Point position;
    Base local;

public:
    PlanetaryStation(const Planet& planet, Real inclination, Real azimuth)
    {
        Transformation t;
        position = t.rotateZ(inclination).rotateY(azimuth).revertBase(planet.local) * Point{0, 1, 0};

        Direction i, j, k;
        Point north = planet.center + (planet.axis / 2.0f);

        k = normalize(position - planet.center);
        i = normalize(cross(north - position, k));
        j = cross(k, i);

        local = Base{i, j, k, position};
    }

    Point getPosition() { return position; }

    const Base& getLocalBase() { return local; }
};

auto getPlanetParams()
{
    Direction axis;
    Point center, refCity;

    std::cout << " - Axis: ";    std::cin >> axis;
    std::cout << " - Center: ";  std::cin >> center;
    std::cout << " - City: ";    std::cin >> refCity;

    return std::tuple{axis, center, refCity};
}

auto getStationParams()
{
    Real azimuth, inclination;

    std::cout << " - Inclination: "; std::cin >> inclination;
    std::cout << " - Azimuth: ";     std::cin >> azimuth;

    return std::tuple{inclination, azimuth};
}

void panic(std::string_view msg)
{
    std::cout << msg; ENDL
    exit(1);
}

int main()
{
    std::cout << "Gimme planet origin";               ENDL
    auto [axis, center, refCity] = getPlanetParams(); ENDL

    auto pack = makePlanetParameterPack(center, refCity, axis);
    if (!pack) panic("Parameters are contradictory!!");
    Planet planet1 {pack.value()};

    std::cout << "Now position of the station";       ENDL
    auto [inclination, azimuth] = getStationParams(); ENDL

    PlanetaryStation station1 {planet1, inclination, azimuth};

    // ------------------------------------------------------- //

    std::cout << "Gimme planet destiny";                 ENDL
    auto [axis2, center2, refCity2] = getPlanetParams(); ENDL

    auto pack2 = makePlanetParameterPack(center2, refCity2, axis2);
    if (!pack2) panic("Parameters are contradictory!!");
    Planet planet2 {pack2.value()};

    std::cout << "Now position of the station";         ENDL
    auto [inclination2, azimuth2] = getStationParams(); ENDL

    PlanetaryStation station2 {planet2, inclination2, azimuth2};

    // ------------------------------------------------------- //

    Point origin = station1.getPosition(), destiny = station2.getPosition();
    Direction dist = destiny - origin;

    std::cout << "Origin station: " << origin; ENDL
    std::cout << "Destiny station: " << destiny; ENDL
    std::cout << "Distance: " << dist; ENDL ENDL

    Transformation toOriginBase, toDestinyBase;
    Direction distOnOrigin = toOriginBase.changeBase(station1.getLocalBase()) * dist;
    Direction distOnDestiny = toDestinyBase.changeBase(station2.getLocalBase()) * (-1.0f * dist);

    std::cout << "Distance on origin: " << distOnOrigin; ENDL
    std::cout << "Distance on destiny: " << distOnDestiny; ENDL

    bool collision = distOnOrigin[2] <= 0.0f || distOnDestiny[2] <= 0.0f;
    if (collision) { std::cout << "COLLISION!!!"; ENDL }
    else { std::cout << "We're safe :)"; ENDL }
}
#include <iostream>
#include <tuple>

#define ENDL std::cout << '\n';

#include "geometry.hpp"
#include "planetary_station.hpp"

auto getPlanetParams()
{
    Direction axis;
    Point center, refCity;

    std::cout << " - Axis:";    std::cin >> axis;
    std::cout << " - Center:";  std::cin >> center;
    std::cout << " - City:";    std::cin >> refCity;

    return std::tuple{axis, center, refCity};
}

auto getStationParams()
{
    Real azimuth, inclination;

    std::cout << " - Inclination:"; std::cin >> inclination;
    std::cout << " - Azimuth:";     std::cin >> azimuth;

    return std::tuple{azimuth, inclination};
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
    if (!pack) panic("Parameters are contradictory!!");
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
    Direction distOnDestiny = toDestinyBase.changeBase(station2.getLocalBase()) * dist;

    std::cout << "Distance on origin: " << distOnOrigin; ENDL
    std::cout << "Distance on destiny: " << distOnDestiny; ENDL

    bool collision = distOnOrigin[2] <= 0.0f && distOnDestiny[2] <= 0.0f;
    if (collision) { std::cout << "COLLISION!!!"; ENDL }
    else { std::cout << "We're safe :)"; ENDL }
}
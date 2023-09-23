#include "planetary_station.hpp"

std::optional<PlanetParameterPack>
makePlanetParameterPack(Point center, Point refCity, Direction axis)
{
    Real radius = norm(refCity - center);
    Real axisHalf = norm(axis) / 2.0f;

    if (std::abs(axisHalf - radius) > 1E-6)
        return {std::nullopt};

    return PlanetParameterPack{center, refCity, axis};
}

Planet::Planet(PlanetParameterPack pack)
    : center(pack.center), refCity(pack.refCity), axis(pack.axis)
{
    Direction i, j, k;
    Direction refRadius = refCity - center;
    Real radius = norm(refRadius);

    j = axis / 2.0f;
    k = normalize(cross(refRadius, j)) * radius;
    i = cross(j, k) / radius;

    local = Base{i, j, k, center};

    assert(local.isBase());
}

PlanetaryStation::PlanetaryStation(const Planet& planet, Real inclination, Real azimuth)
{
    Transformation t;
    position = t.rotateZ(inclination).rotateY(azimuth).revertBase(planet.local) * Point{0, 1, 0};

    /*
    Transformation Rz, Ry, Br;
    Point p {0, 1 ,0};
    std::cout << "north: " << p << '\n';
    p = Rz.rotateZ(inclination) * p;
    std::cout << "rotatedZ: " << p << '\n';
    p = Ry.rotateY(azimuth) * p;
    std::cout << "rotatedY: " << p << '\n';
    p = Br.revertBase(planet.local) * p;
    std::cout << "p: " << p << '\n';
    */

    Direction i, j, k;
    Point north = planet.center + (planet.axis / 2.0f);

    k = normalize(position - planet.center);
    i = normalize(cross(north - position, k));
    j = cross(k, i);

    local = Base{i, j, k, position};

    assert(local.isBase());
}

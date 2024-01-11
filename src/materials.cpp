#include "materials.hpp"

#include <tuple>

Direction uniformCosineSampling(const Direction& normal, Randomizer& random)
{
    const Direction ortogonal1 = (std::abs(normal[0]) < 0.1)
        ? normalize(Direction{0, normal[2], -normal[1]})
        : normalize(Direction{normal[1], -normal[0], 0});

    const Direction ortogonal2 = cross(ortogonal1, normal);

    const Real sin2Lat = random();
    const Real cosLat = std::sqrt(1 - sin2Lat);
    const Real sinLat = std::sqrt(sin2Lat);

    const Real az = 2 * numbers::pi * random();
    const Real sinAz = std::sin(az);
    const Real cosAz = std::cos(az);

    const Direction rotatedDirection = normal * cosLat 
                                     + ortogonal2 * (sinLat * cosAz)
                                     + ortogonal1 * (sinLat * sinAz);

    return rotatedDirection;
}

Direction perfectSpecularReflexion(const Direction& normal, const Direction& dir)
{
    return dir - 2 * normal * dot(dir, normal);
}

Direction perfectSpecularRefraction(const Shape::Normal& normal, const Direction& dir, Real refIndex) 
{
    const Direction n = -1 * normal.normal;
    const Real index = normal.side == Shape::Side::in ? refIndex : 1 / refIndex;
    
    const Real cosOut = dot(dir, n);

    /* if (std::abs(cosOut) > 0.99999)
        return n; */

    const Real sinIn = std::sin(std::acos(cosOut)) * index;
    const Real in = std::asin(sinIn);
    const Real cosIn = std::cos(in);

    const Direction rotRef = normalize(cross(n, dir));
    const Direction ortogonal2 = cross(rotRef, n);

    const Direction rotatedDirection = n * cosIn + ortogonal2 * sinIn;

    return normalize(rotatedDirection);
}

Material::Evaluation Material::eval(const Point& hit, const Ray& wIn, Ray& wOut,
        const Shape::Normal& normal, Randomizer& random) const
{
    // Russian Roulette
    Real pd = _kd.luminance(), ps = _ks.luminance(), pt = _kt.luminance();
    if (const Real sum = pd + ps + pt; sum > 1)
    {
        const Real divisor = 1.1 * sum; // leave absorption probability
        pd /= divisor; ps /= divisor; pt /= divisor;
    }

    auto setWOut = [&](const Direction& dir)
    {
        wOut.p = hit + dir * 0.0001;
        wOut.d = dir;
    };

    Real x = random();
    if (x <= pd)
    {
        auto dir = uniformCosineSampling(normal.normal, random);
        setWOut(dir);
        return {_kd / pd, Component::kd};
    }
    else if (x - pd <= ps)
    {
        setWOut(perfectSpecularReflexion(normal.normal, wIn.d));
        return {_ks / ps, Component::ks};
    }
    else if (x - pd - ps <= pt)
    {   
        setWOut(perfectSpecularRefraction(normal, wIn.d, index));
        return {_kt / pt, Component::kt};
    }

    return {Color{}, Component::ka};
}

Material::Sample Material::sample(const Point& hit, const Ray& wIn,
        const Shape::Normal& normal, Randomizer& random) const
{
    // Russian Roulette
    Real pd = _kd.luminance(), ps = _ks.luminance(), pt = _kt.luminance();
    
    const Real sum = pd + ps + pt;
    pd /= sum; ps /= sum; pt /= sum;

    auto setWOut = [&](const Direction& dir) -> Ray
    {
        return {hit + dir * 0.0001, dir};
    };

    Real x = random();
    if (x <= pd)
    {
        auto dir = uniformCosineSampling(normal.normal, random);
        return {setWOut(dir), Component::kd};
    }
    else if (x - pd <= ps)
    {
        auto dir = perfectSpecularReflexion(normal.normal, wIn.d);
        return {setWOut(dir), Component::ks};
    }
    else
    {
        auto dir = perfectSpecularRefraction(normal, wIn.d, index);
        return {setWOut(dir), Component::kt};
    }
}

Material::SampleAll Material::sampleAll(const Point& hit, const Ray& wIn,
        const Shape::Normal& normal, Randomizer& random) const
{
    SampleAll res;
    const Real pd = _kd.luminance(), ps = _ks.luminance(), pt = _kt.luminance();
    const Real sum = pd + ps + pt;
    res.pd = pd / sum; res.ps = ps / sum; res.pt = pt / sum;

    auto setWOut = [&](const Direction& dir) -> Ray
    {
        return {hit + dir * 0.0001, dir};
    };

    res.rd = setWOut(uniformCosineSampling(normal.normal, random));
    res.rs = setWOut(perfectSpecularReflexion(normal.normal, wIn.d));
    res.rt = setWOut(perfectSpecularRefraction(normal, wIn.d, index));

    return res;
}
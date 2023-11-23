#pragma once

#include "materials.hpp"

inline Material emitter(const Color& ke)
{
    Material m;
    m.emits = true;
    m._kd = ke;
    return m;
}

inline Material diffuse(const Color& kd)
{
    Material m;
    m._kd = kd;
    return m;
}

inline Material specular(const Color& ks)
{
    Material m;
    m._ks = ks;
    return m;
}

inline Material refractive(const Color& kt, Real index)
{
    Material m;
    m._kt = kt;
    m.index = index;
    return m;
}

inline Material operator+(const Material& a, const Material& b)
{
    Material mix;
    mix.emits = a.emits | b.emits;
    mix._kd = a._kd + b._kd;
    mix._ks = a._ks + b._ks;
    mix._kt = a._kt + b._kt;
    mix.index = a.index + b.index;
    return mix;
}

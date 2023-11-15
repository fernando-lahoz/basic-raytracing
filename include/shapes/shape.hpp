#pragma once

#include "shading.hpp"
#include "geometry.hpp"
#include "ray_tracing.hpp"

struct Material
{
    bool emits;
    Color kd, ks, kt;
    Real hIndex; 

    static inline Material emitter(const Color& ke)
    {
        return {.emits = true, .kd = ke, .ks = {}, .kt = {}};
    }
};

enum class Side {in, out};

struct NormalReturn
{
    Side from;
    Direction normal;
};

class Shape 
{
protected:
    Material _material;

    inline Shape (const Material& material_)
        : _material{material_} {}
public:
    virtual Real intersect(const Ray& ray) const = 0;

    virtual NormalReturn normal(const Direction d, const Point hit) const = 0;

    inline const Material& material() const { return _material; }
};
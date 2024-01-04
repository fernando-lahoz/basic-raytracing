#pragma once

#include "shading.hpp"
#include "geometry.hpp"
#include "random.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"

class Material
{
private:
    bool emits = false;
    Color _kd, _ks, _kt;
    Real index = 0; 
public:

    friend Material emitter(const Color& ke);
    friend Material diffuse(const Color& kd);
    friend Material specular(const Color& ks);
    friend Material refractive(const Color& kt, Real index);

    friend Material operator+(const Material& a, const Material& b);

    inline Color kd() const { return _kd; }
    inline Color ks() const { return _ks; }
    inline Color kt() const { return _kt; }

    enum class Component : uint8_t {ka, kd, ks, kt};

    struct Evaluation
    {
        Color color;
        Component comp;
    };

    Material::Evaluation eval(const Point& hit, const Ray& wIn, Ray& wOut,
            const Shape::Normal& normal, Randomizer& random) const;

    struct Emission
    {
        bool emits;
        Color ke;
    };

    inline Emission emission() const
    {
        return {emits, _kd};
    }

    struct Sample
    {
        Ray ray;
        Component comp;
    };

    Sample sample(const Point& hit, const Ray& wIn,
            const Shape::Normal& normal, Randomizer& random) const;

    struct SampleAll
    {
        Ray rd, rs, rt;
        Real pd, ps, pt;
    };

    SampleAll sampleAll(const Point& hit, const Ray& wIn,
            const Shape::Normal& normal, Randomizer& random) const;
};

#include "inline/materials.ipp"
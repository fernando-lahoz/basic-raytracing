#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "geometry.hpp"
#include "object_set.hpp"

#include <iostream>

Camera::Camera(Point pinhole, Direction front, Direction up, Dimensions dim)
    : f { front },
      l { normalize(cross(up, front)) * ((norm(up) * dim.width) / dim.height) },
      u { up },
      o { pinhole },
      pixelWidth{ 2.0 / dim.width }, pixelHeight{ 2.0 / dim.height },
      randomX{0, pixelWidth}, randomY{0, pixelHeight},
      rd{}
{
    gen.seed(rd());
}

Camera::Camera(const Camera& cam)
    : f{cam.f}, l{cam.l}, u{cam.u}, o{cam.o},
      pixelWidth{cam.pixelWidth}, pixelHeight{cam.pixelHeight},
      randomX{0, pixelWidth}, randomY{0, pixelHeight},
      rd{}
{
    gen.seed(rd());
}

Ray Camera::randomRay(Index i, Index j)
{
    Real y = static_cast<Real>(1 - pixelHeight * i - randomY(gen));
    Real x = static_cast<Real>(1 - pixelWidth * j - randomX(gen));

    return {o, normalize((x * l) + (y * u) + f)};
}

Color castShadowRays(const ObjectSet& objSet, const Direction& normal,
        const Point& hit, const Color& kd)
{
    Color color {0, 0, 0};
    for (const PointLight& light : objSet.pointLights)
    {
        const Direction d = light.position() - hit;
        const Real d2 = dot(d, d);
        const Real distance = std::sqrt(d2);
        const Direction dN = d / distance; // normalized d
        
        const Direction epsilon = dN * 0.0001;

        const Ray shadowRay {hit + epsilon, dN};
        auto isThereNearerObject = [&](Real distance) -> bool
        {
            for (const Object& obj : objSet.objects)
            {
                const auto its = obj.shape().intersect(shadowRay);
                if (Ray::isHit(its) && its < distance)
                    return true;
            }
            return false;
        };

        if (isThereNearerObject(distance))
            continue;

        const Color emission = light.color() / d2;
        const Real term = std::abs(dot(normal, dN));
        color = color + (emission * kd / numbers::pi) * term;
    }
    return color;
}

Intersection findIntersection(const ObjectSet& objSet, const Ray& ray)
{
    Real t = Ray::nohit;
    const Object *hitObj = nullptr;
    for (const Object& obj : objSet.objects)
    {
        const auto its = obj.shape().intersect(ray);
        if (Ray::isHit(its) && (!Ray::isHit(t) || its < t))
        {
            t = its;
            hitObj = &obj;
        } 
    }
    return {t, hitObj};
}

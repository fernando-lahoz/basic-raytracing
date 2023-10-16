#pragma once

#include "geometry.hpp"
#include "image.hpp"
#include "shapes.hpp"

#include <ranges>
#include <vector>
#include <random>

struct Ray
{
    Point p;
    Direction d; // must be normalized

    static inline constexpr Real nohit = -1;

    inline Point hitPoint(Real t) { return p + d * t; }
};

class Camera
{
private:
    Direction f, l, u;
    Point o;
    Real width, height;
public:
    Camera(Point pinhole, Direction front, Direction up)
        : o { pinhole },
          f { front },
          l { normalize(cross(up, front)) },
          u { up } {}

    void render(Image& img, std::vector<std::reference_wrapper<const Shape>> objects) const
    {
        auto [width, height] = img.dimensions();

        Direction left = l * ((norm(u) * width) / height);

        Transformation cameraToWorld;
        cameraToWorld.revertBase(Base{f, left, u, o});

        Real pixelWidth = 1 / width;
        Real pixelHeight = 1 / height;

        Real xmin = 0, ymin = 0;

        std::random_device rd;  // Se utilizará para sembrar el generador de aleatorios
        std::mt19937 gen {rd()};
        std::uniform_real_distribution<Real> randomX {0, pixelWidth};
        std::uniform_real_distribution<Real> randomY {0, pixelHeight};
        
        for (Index i : std::views::iota(Index{0}, height))
        {
            for (Index j : std::views::iota(Index{0}, width))
            {
                Direction d {1, xmin + randomX(gen), ymin + randomY(gen)};
                Ray ray {o, cameraToWorld * d};

                Real minT = 0;
                const Shape* hitObj = nullptr;
                for (const Shape& obj : objects)
                {
                    Real t = obj.intersect(ray);
                    if (t == Ray::nohit)
                        continue;
                    if (t < minT)
                    {
                        minT = t;
                        hitObj = &obj;
                    }
                }

                Emission color = {0, 0, 0};
                if (hitObj != nullptr)
                    color = hitObj->color();

                img(i, j) = RGBPixel{color};

                xmin += pixelWidth;
            }
            ymin += pixelHeight;
        }
    }
};
#include "raytracing.hpp"
#include "shapes.hpp"
#include "geometry.hpp"

#include <ranges>
#include <vector>
#include <random>

#include <iostream>

Camera::Camera(Point pinhole, Direction front, Direction up)
    : f { front },
      l { normalize(cross(up, front)) },
      u { up },
      o { pinhole } {}

void Camera::pathtrace(Image& img, const ObjectSet& objects, Index ppp) const
{
    auto [width, height] = img.dimensions();

    Direction left = l * ((norm(u) * width) / height);

    Transformation cameraToWorld;
    cameraToWorld.revertBase(Base{f, left, u, o});

    Real pixelWidth = 2.0 / width;
    Real pixelHeight = 2.0 / height;

    std::random_device rd;
    std::mt19937 gen {rd()};
    std::uniform_real_distribution<Real> randomX {0, pixelWidth};
    std::uniform_real_distribution<Real> randomY {0, pixelHeight};

    Real ymin = 1;
    for (Index i : std::views::iota(Index{0}, height))
    {
        Real xmin = 1;
        for (Index j : std::views::iota(Index{0}, width))
        {
            auto trace = [&](Ray ray, ObjectSet objects) -> Emission
            {
                Real minT = Ray::nohit;
                const Shape* hitObj = nullptr;
                for (const Shape& obj : objects)
                {
                    Real t = obj.intersect(ray);
                    if (t == Ray::nohit)
                        continue;
                    if (minT == Ray::nohit || t < minT)
                    {
                        minT = t;
                        hitObj = &obj;
                    }
                }

                return (hitObj != nullptr) ? hitObj->color() : Emission{0, 0, 0};
            };

            Emission meanColor {0, 0, 0};

            for ([[maybe_unused]] Index k : std::views::iota(Index{0}, ppp))
            {
                Real x = xmin - randomX(gen);
                Real y = ymin - randomY(gen);
                Ray ray {o, normalize(cameraToWorld * Direction{1, x, y})};
                Emission color = trace(ray, objects);
                meanColor.r += color.r;
                meanColor.g += color.g;
                meanColor.b += color.b;

                // std::cout << "Launching ray at pixel (" << i << ", " << j << "):\t";
                // std::cout << "{" << x << ", " << y << "} -->" << ray.d << '\n';
            }

            img(i, j) = RGBPixel{meanColor.r / ppp, meanColor.g / ppp, meanColor.b / ppp};

            xmin -= pixelWidth; // dudoso método
        }
        ymin -= pixelHeight;
    }
}
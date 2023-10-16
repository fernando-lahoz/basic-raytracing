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

void Camera::render(Image& img, const std::vector<std::reference_wrapper<const Shape>>& objects) const
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

    // std::cout << "pixelWidth: " << pixelWidth << '\n';
    // std::cout << "pixelHeight: " << pixelHeight << '\n';
    
    Real ymin = 1;
    for (Index i : std::views::iota(Index{0}, height))
    {
        Real xmin = 1;
        for (Index j : std::views::iota(Index{0}, width))
        {
            Real x = xmin - randomX(gen);
            Real y = ymin - randomY(gen);
            Direction d {1, x, y};
            Ray ray {o, normalize(cameraToWorld * d)};

            // std::cout << "Launching ray at pixel (" << i << ", " << j << "):\t";
            // std::cout << "{" << d[1] << ", " << d[2] << "} -->" << ray.d << '\n';

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

            Emission color = {0, 0, 0};
            if (hitObj != nullptr)
                color = hitObj->color();

            img(i, j) = RGBPixel{color};

            xmin -= pixelWidth; // dudoso método
        }
        ymin -= pixelHeight;
    }
}
#pragma once

#include <vector>

#include "shapes.hpp"
#include "light.hpp"

struct ObjectSet
{
    std::vector<std::reference_wrapper<const Shape>> objects;
    std::vector<std::reference_wrapper<const PointLight>> pointLights;
};
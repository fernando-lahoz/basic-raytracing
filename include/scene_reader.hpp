#pragma once

#include "object_set.hpp"

#include <optional>
#include <string>
#include <sstream>
#include <fstream>

struct Scene
{
    Point focus;
    Direction front, up;
    ObjectSet objects; 
};

std::optional<Scene> makeSceneFromFile(std::string_view file_name);
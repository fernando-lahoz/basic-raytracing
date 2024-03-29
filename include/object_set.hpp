#pragma once

#include <vector>
#include <memory>

#include "shapes.hpp"
#include "light.hpp"
#include "materials.hpp"

class Object
{
private:
    std::shared_ptr<const Shape> _shape;
    std::shared_ptr<const Material> _material;
public:
    inline Object(std::shared_ptr<const Shape> shape_,
            std::shared_ptr<const Material> material_)
        : _shape{shape_}, _material{material_}
    {}

    inline const Shape& shape() const { return *(_shape.get()); }

    inline const Material& material() const { return *(_material.get()); }
};

struct ObjectSet
{
    std::vector<Object> objects;
    std::vector<PointLight> pointLights;
};
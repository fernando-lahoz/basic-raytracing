#include <optional>

#include "geometry.hpp"

struct Ray
{
    Point p;
    Direction d; // must be normalized
};

struct Intersection
{
    Point intersection;
    Direction normal;
};

class Shape 
{
public:
    virtual std::optional<Intersection> intersect(Ray ray) const = 0;
};
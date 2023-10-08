#include "geometry.hpp"

struct Ray
{
    Point p;
    Direction d; // must be normalized
};

struct IntersectResult
{
    Point intersection;
    Direction normal;
    bool intersects;
};

class Shape 
{
public:
    virtual IntersectResult intersect(Ray ray) const = 0;
};
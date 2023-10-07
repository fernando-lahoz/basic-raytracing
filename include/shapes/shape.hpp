#include "geometry.hpp"

struct Ray
{
    Point p;
    Direction d;
};

union IntersectResult
{
    Point intersection;
    Real check;

    bool intersects() { return std::isinf(check); }
};

class Shape 
{
public:
    virtual IntersectResult intersect(Ray ray) const = 0;
};
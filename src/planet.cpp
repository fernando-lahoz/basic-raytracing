#include <geometry.hpp>

class Planet
{
    protected:
        Point center;
        Point refCity;
        Direction axis;

    public:

        Planet(Point _center, Point _refCity, Direction _axis)
            :   center(_center), refCity(_refCity), axis(_axis)
        {}




};
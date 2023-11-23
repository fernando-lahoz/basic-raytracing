#pragma once

#include "shapes/plane.hpp"

class DiskBorder
{
protected:  
    Real r;
public:
    DiskBorder (Real radius) : r{radius} {}
    inline bool isInside(const Point p, const LimitedPlane<DiskBorder>& plane) const;
};

CHECK_BORDER_CONCEPT(DiskBorder)

class Disk : public LimitedPlane<DiskBorder>
{
public:
    Disk(Direction normal, Point center, Real radius)
        : LimitedPlane{center, normal, DiskBorder{radius}} {}
};

#include "shapes/disk.ipp"
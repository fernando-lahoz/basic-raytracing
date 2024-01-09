#pragma once

#include "shapes/disk.hpp"
#include "shapes/plane.hpp"

bool DiskBorder::isInside(const Point p, const LimitedPlane<DiskBorder>& plane) const
{
    return (norm(p - plane.o) < r);
}
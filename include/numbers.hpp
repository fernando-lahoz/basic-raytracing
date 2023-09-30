#pragma once

#include <cfloat>
#include <sstream>
#include <iomanip>

using Real = float;
using Natural = unsigned long long;
using Integer = long long;

using Index = std::size_t;

std::string toString(Real x)
{
    std::size_t precision;
    if constexpr (std::is_same<Real, float>{}) precision = FLT_DECIMAL_DIG;
    else if constexpr (std::is_same<Real, double>{}) precision = DBL_DECIMAL_DIG;
    else if constexpr (std::is_same<Real, long double>{}) precision = LDBL_DECIMAL_DIG;

    std::stringstream ss;
    ss << std::setprecision(precision) << x;
    return ss.str();
}

Real mod (Real x, Natural n)
{
    return x - n * static_cast<Natural>(x / n);
};

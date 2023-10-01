#include "numbers.hpp"

std::string numbers::ToString(Real x)
{
    std::size_t precision = FLT_DIG;
    /*
    if constexpr (std::is_same<Real, float>{}) precision = FLT_DIG;
    else if constexpr (std::is_same<Real, double>{}) precision = DBL_DIG;
    else if constexpr (std::is_same<Real, long double>{}) precision = LDBL_DIG;
    */

    std::stringstream ss;
    ss << std::setprecision(precision) << x;
    return ss.str();
}

Real numbers::mod(Real x, Natural n)
{
    return x - n * static_cast<Natural>(x / n);
};
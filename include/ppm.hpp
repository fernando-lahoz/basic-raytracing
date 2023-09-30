#pragma once

#include <vector>
#include <istream>
#include <ostream>
#include <string>
#include <numeric>
#include <variant>
#include <charconv>
#include <cctype>
#include <ranges>
#include <tuple>
#include <concepts>
#include <sstream>

#include "definitions.hpp"

namespace ppm {

using ErrorMsg = std::string;

class Image
{
private:
    std::vector<Real> redBuffer;
    std::vector<Real> greenBuffer;
    std::vector<Real> blueBuffer;

    unsigned long long colorRes;
    Real maxValue;
    std::size_t nRows, nColumns;

public:

    friend bool read(std::istream& is, ErrorMsg& error, Image& img);
};

[[nodiscard]] bool read(std::istream& is, ErrorMsg& error, Image& img);

} //namespace ppm

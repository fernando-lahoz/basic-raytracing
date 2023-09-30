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
#include <iomanip>

#include "real_numbers.hpp"

namespace ppm {

using ErrorMsg = std::string;

class Image
{
private:
    std::vector<Real> redBuffer;
    std::vector<Real> greenBuffer;
    std::vector<Real> blueBuffer;
    
    Real maxValue;
    std::size_t nRows, nColumns;
    bool foundMaxValue;

public:
    unsigned long long colorRes;

    friend bool read(std::istream& is, ErrorMsg& error, Image& img);
    friend void write(std::ostream& os, const Image& img);
};

[[nodiscard]] bool read(std::istream& is, ErrorMsg& error, Image& img);

void write(std::ostream& os, const Image& img);

} //namespace ppm

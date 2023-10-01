#pragma once

#include <cfloat>
#include <sstream>
#include <iomanip>

using Real = float;
using Natural = unsigned long long;
using Integer = long long;

using Index = std::size_t;

namespace numbers {

std::string ToString(Real x);

Real mod(Real x, Natural n);

template <typename Ty>
auto max(Ty n) { return n; }

template <typename Ty, typename ...Args>
auto max(Ty n, Args ...args) {
    auto m = max(args...);
    return n > m ? n : m;
}

template <typename Ty>
auto min(Ty n) { return n; }

template <typename Ty, typename ...Args>
auto min(Ty n, Args ...args) {
    auto m = min(args...);
    return n < m ? n : m;
}

} //namespace numbers

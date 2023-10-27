#pragma once

#include <cfloat>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ranges>

using Real = float;
using Natural = unsigned long long;
using Integer = long long;

using Index = std::size_t;

namespace numbers {

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

using Range = std::ranges::iota_view<Index, Index>;
constexpr inline Range range(Index start, Index end)
{
    return std::views::iota(start, end);
}

} //namespace numbers

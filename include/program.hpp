#pragma once

#include <iostream>

#define SET_PROGRAM_NAME(argv) program::name = argv[0]

namespace program {

static std::string_view name;

struct DirectExit{ const std::ostream& out; int returnVal; };

const DirectExit direct = DirectExit{std::cout, 0};

struct ErrorExit{ const std::ostream& out; int returnVal; };

ErrorExit err(int returnVal = 1) { return ErrorExit{std::cerr, returnVal}; }

[[noreturn]] void exit(DirectExit ex)
{
    const_cast<std::ostream&>(ex.out) << std::endl;
    std::exit(ex.returnVal);
}

//std::cerr
template <typename Ty, typename ...Args>
void exit(DirectExit ex, Ty head, Args&& ...tail)
{
    const_cast<std::ostream&>(ex.out) << head;
    program::exit(ex, std::forward<Args>(tail)...);
}

template <typename Ty, typename ...Args>
void exit(ErrorExit ex, Ty head, Args&& ...tail)
{
    const_cast<std::ostream&>(ex.out) << name << ": " << head;
    program::exit(DirectExit{ex.out, ex.returnVal}, std::forward<Args>(tail)...);
}

template <typename Ty, typename ...Args>
void error(int returnVal, Ty head, Args&& ...tail)
{
    std::cerr << name << ": " << head;
    program::exit(DirectExit{std::cerr, returnVal}, std::forward<Args>(tail)...);
} 

} //program

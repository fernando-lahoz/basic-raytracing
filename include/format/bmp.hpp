#pragma once

#include <istream>
#include <ostream>

class Image;

namespace bmp {

[[nodiscard]] bool read(std::istream& is, Image& img);

void write(std::ostream& os, const Image& img);

} //namespace bmp

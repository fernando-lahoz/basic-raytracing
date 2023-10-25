#pragma once

#include <istream>
#include <ostream>

class Image;

namespace ppm {

[[nodiscard]] bool read(std::istream& is, Image& img);

[[nodiscard]] bool write(std::ostream& os, const Image& img);

} //namespace ppm

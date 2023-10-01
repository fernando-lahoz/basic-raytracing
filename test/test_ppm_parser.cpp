#include <iostream>
#include <fstream>

#include "image.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    std::ifstream input {argv[1]};
    if (!input.is_open())
    {   
        std::cout << "Could not open origin file\n";
        return 1;
    }
    
    std::ofstream output {argv[2]};
    if (!output.is_open())
    {
        std::cout << "Could not open destination file\n";
        return 1;
    }

    Image img;
    ErrorMsg error;
    if (!ppm::read(input, img))
    {
        std::cout << error << std::endl;
        return 1;
    }
    img.colorResolution = 255;
    img.foundMaxValue = false;
    img.maxLuminance = 255;
    ppm::write(output, img);

    return 0;
}
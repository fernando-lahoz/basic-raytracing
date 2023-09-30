#include <iostream>
#include <fstream>

#include "ppm.hpp"

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

    ppm::Image img;
    ppm::ErrorMsg error;
    if (!ppm::read(input, error, img))
    {
        std::cout << error << std::endl;
        return 1;
    }


    return 0;
}
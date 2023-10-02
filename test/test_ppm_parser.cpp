#include <iostream>
#include <fstream>

#include "image.hpp"
#include "image_reader.hpp"
#include "image_writer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    auto reader = makeImageReader(argv[1]);
    if (reader == nullptr)
    {   
        std::cout << "Could not open origin file\n";
        return 1;
    }
    
    Image img; 
    if (!reader.get()->read(img))
    {
        std::cout << "error" << std::endl;
        return 1;
    }
    
    auto writer = makeImageWriter<PPMWriter>(argv[2]);
    if (writer == nullptr)
    {
        std::cout << "Could not open destination file\n";
        return 1;
    }

    //img.toneMap(Clamping{});
    //img.toneMap(Equalization{img.luminance()});
    //img.toneMap(Equalization_Clamping{1.7323});
    //img.toneMap(Gamma{img.luminance(), 0.2});
    img.toneMap(Gamma_Clamping{4, 0.5});
    img.changeResolution(255);
    writer.get()->write(img);

    return 0;
}
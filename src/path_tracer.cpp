#include "scenes/cornell_box_1.ipp"

#include "image.hpp"
#include "image_writer.hpp"

#include <iostream>

int main()
{
    const auto& camera = cornell_box_1::camera;
    const auto& objects = cornell_box_1::objects;

    Image img {1, 255, Dimensions{500, 500}};
    camera.pathtrace(img, objects, 10);

    auto writer = makeImageWriter("ornell_box_1.bmp", "bmp");
    if (writer == nullptr)
        std::cout << "Could not open destination file or format not available." << std::endl;

    writer->write(img);

    return 0;
}
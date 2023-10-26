#include "scenes/cornell_box_1.ipp"

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"

#include <iostream>

// Change namespace to change the scene
using namespace cornell_box_1;

int main()
{
    const Dimensions dimensions {500, 500};

    Camera camera {cam::focus, cam::front, cam::up, dimensions};
    Image img {1, 255, dimensions};

    auto divider = std::make_unique<PixelDivider>(dimensions);
    PathTracingThreadPool pathTracer {
        PathTracingThreadPool::totalConcurrency,
        100, std::move(divider)
    };

    auto writer = makeImageWriter("cornell_box_1.bmp", "bmp");
    if (writer == nullptr)
    {
        std::cout << "Could not open destination file or format not available." << std::endl;
        return 1;
    }
    
    pathTracer.render(camera, img, objects, 20);

    if (!writer->write(img))
    {
        std::cout << "Could not write destination file." << std::endl;
        return 1;
    }

    return 0;
}
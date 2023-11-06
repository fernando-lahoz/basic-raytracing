#include "scenes/cornell_box_test.ipp"
#include "scenes/cornell_box_1.ipp"

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"

#include "program.hpp"

// Change namespace to change the scene
using namespace cornell_box_test;

int main(int argc, char* argv[])
{
    SET_PROGRAM_NAME(argv);
    const Dimensions dimensions {256, 256}; //-d 500:500
    const Natural resolution = (Natural{1} << 32) - 1; //8bit/16bit/32bit if bmp
    const Natural ppp = 10; //-ppp 20
    const Dimensions taskDivision {10, 10}; //--task-division=region:10:10/row/column/pixel
    //const std::string_view numThreads = "--task-concurrency=total"; //1,2...
    //const std::string_view queue = "--task-size=unbounded"; //20,50...
    const std::string_view destination = "cornell_box_test.ppm";
    const std::string_view format = "ppm"; //--output-format=bmp

    Camera camera {cam::focus, cam::front, cam::up, dimensions};
    Image img {1, resolution, dimensions};

    PathTracingThreadPool pathTracer {
        PathTracingThreadPool::totalConcurrency, 100,
        TaskDivider{dimensions, taskDivision}
    };

    auto writer = makeImageWriter(destination, format);
    if (writer == nullptr)
        program::exit(program::err(), "Could not open destination file or format not available.");
    
    pathTracer.render(camera, img, objects, ppp);

    img.updateLuminance();

    if (!writer->write(img))
        program::exit(program::err(), "Could not write destination file.");

    return 0;
}
#include "scenes/cornell_box_1.ipp"

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"

#include "program.hpp"

// Change namespace to change the scene
using namespace cornell_box_1;

int main(int argc, char* argv[])
{
    SET_PROGRAM_NAME(argv);
    const Dimensions dimensions {500, 500}; //-d 500:500
    const Natural resolution = 255; //8bit/16bit/32bit if bmp
    const Natural ppp = 20; //-ppp 20
    // luminance ??? (maybe generated??)
    //const std::string_view taskDivider = "--task-division=region:10:10"; //pixel/row/column
    //const std::string_view numThreads = "--task-concurrency=total"; //1,2...
    //const std::string_view queue = "--task-size=unbounded"; //20,50...
    
    const std::string_view destiny = "cornell_box_1.bmp";
    const std::string_view format = "bmp"; //--output-format=bmp

    Camera camera {cam::focus, cam::front, cam::up, dimensions};
    Image img {1, resolution, dimensions};

    auto divider = std::make_unique<RegionDivider>(dimensions, Dimensions{10, 10});
    PathTracingThreadPool pathTracer {
        PathTracingThreadPool::totalConcurrency,
        100, std::move(divider)
    };

    auto writer = makeImageWriter(destiny, format);
    if (writer == nullptr)
        program::exit(program::err(), "Could not open destination file or format not available.");
    
    pathTracer.render(camera, img, objects, ppp);

    if (!writer->write(img))
        program::exit(program::err(), "Could not write destination file.");

    return 0;
}
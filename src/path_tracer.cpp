#include "scenes/cornell_box_test.ipp"
#include "scenes/cornell_box_1.ipp"

#include <chrono>
#include <iostream>

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"

#include "program.hpp"

// Change namespace to change the scene
using namespace cornell_box_test;

double measure(auto lambda)
{
    auto start = std::chrono::system_clock::now();

    lambda();

    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
}

int main(int argc, char* argv[])
{
    SET_PROGRAM_NAME(argv);
    const Dimensions dimensions {256, 256}; //-d 500:500
    const Natural resolution = (Natural{1} << 32) - 1; //8bit/16bit/32bit if bmp
    const Natural ppp = 50; //-ppp 20
    const Dimensions taskDivision {10, 10}; //--task-division=region:10:10/row/column/pixel
    const std::string_view destination = "cornell_box_test.ppm";
    const std::string_view format = "ppm"; //--output-format=bmp
    const Index taskConcurrency = 0;//PathTracingThreadPool::totalConcurrency; //"--task-concurrency=total"; //1,2...
    const Index taskQueueSize = 100;//"--task-queue-size=unbounded"; //20,50...

    Camera camera {cam::focus, cam::front, cam::up, dimensions};
    Image img {1, resolution, dimensions};

    PathTracingThreadPool pathTracer {taskConcurrency, taskQueueSize,
            TaskDivider{dimensions, taskDivision}};

    auto writer = makeImageWriter(destination, format);
    if (!writer)
        program::exit(program::err(), "Could not open destination file or format not available.");
    
    std::cout << "Parallelization level: " << pathTracer.numThreads() << '\n';

    const auto seconds = measure([&]()
    {
        pathTracer.render(camera, img, objects, ppp);
    });

    std::cout << "Render finished in " << seconds << " s\n";

    img.updateLuminance();

    if (!writer->write(img))
        program::exit(program::err(), "Could not write destination file.");

    return 0;
}
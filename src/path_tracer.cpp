#include "scenes/cornell_box_test.ipp"
//#include "scenes/cornell_box_1.ipp"

#include <chrono>
#include <iostream>

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"

#include "program.hpp"

static constexpr std::string_view helpStr = R"(
Usage: ./path_tracer [OTPION...] OUTPUT_FILE
    
  -d, --dimensions=WIDTH:HEIGTH    Set dimensions of the image.
                                   Default size is 256x256.

  -r, --resolution=INT             Set range of natural values each
                                   RGB component has to be stored.
                                   Default resolution is 8 bits (LDR).

      For HDR images it is recommended to use a resolution over 2^24.

      Images with bitmap (bmp) output format must use one of these values:
        8bit       (255)
        16bit      (65535)
        32bit      (4294967295)

  -f, --output-format=STRING       Save image with this format.
                                   If not specified, it is deduced
                                   from the file extension.
                            
      Available formats: ppm, bmp

  -p, --paths-per-pixel=INT        Set the number of samples per pixel.
                                   Default value is 50.

  -t, --task-division=STRING       Array of pixels that compounds a task.
      
      Available divisions: region:WIDTH:HEIGTH, row, column, pixel

  -c, --task-concurrency=INT       Set the number of worker threads.
                                   By default, it uses CPU total
                                   number of logical threads.

      Special values: none (1), total (0)

  -q, --task-queue-size=INT        Set the number of entries of the
                                   task queue. Default size is 100.
)";

auto usage(int argc, char *argv[])
{
    auto checkOpt = [](std::string_view str,
            std::string_view opt1, std::string_view opt2) -> Index
    {
        Index len1 = opt1.length(), len2 = opt2.length();
        if (str.substr(0, len1) == opt1) return len1;
        else if (str.substr(0, len2) == opt2) return len2;
        return 0;
    };

     for (int i = 1; i < argc; ++i)
    {
        std::string_view str {argv[i]};
        if (Index pos = checkOpt(str, "-h", "--help"); pos > 0)
        {
            program::exit(program::direct, helpStr);
        }
    }
}

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
    usage(argc, argv);
    const Dimensions dimensions {2000, 2000}; //-d 500:500
    const Natural resolution = (Natural{1} << 32) - 1; //8bit/16bit/32bit if bmp
    const Natural ppp = 2000; //-ppp 20
    const Dimensions taskDivision {1, 1}; //--task-division=region:10:10/row/column/pixel
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
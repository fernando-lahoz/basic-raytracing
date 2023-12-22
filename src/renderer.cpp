#include <chrono>
#include <iostream>
#include <functional>
#include <initializer_list>
#include <charconv>

#include "image.hpp"
#include "image_writer.hpp"
#include "path_tracing.hpp"
#include "photon_mapping.hpp"
#include "scene_reader.hpp"

#include "program.hpp"

enum class Algorithm {photon_mapping, path_tracing};

static constexpr std::string_view helpStr = R"(
Usage: ./renderer [OTPION...] SCENE_FILE OUTPUT_FILE

  -d, --dimensions=WIDTH:HEIGTH    Set dimensions of the image.
                                   Default size is 256x256.

  -c, --color-resolution=INT       Set range of natural values each
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


Ray tracing common parameters:

  -a, --algorithm=STRING           Set the ray tracing algorithm.

      Available algorithms: path-tracing (pt), photon-mapping (pm)

  -p, --paths-per-pixel=INT        Set the number of samples per pixel.
                                   Default value for path tracing is 100,
                                   whereas for photon mapping the default
                                   value is 10.


Path tracing special parameters:

  -s, --path-tracing-strategy=STRING    Path tracing algorithm traces
                                        indirect light using a recursive
                                   implementation by default. Set this
                                   parameter to change the path tracer
                                   implementation.
                                        
      Available implementations:
        trace-projection    ->  Considers every object is an area light
        trace-direct-light  ->  Only traces direct light
        recursive           ->  Traces indirect light recursively
        iterative           ->  Traces indirect light iteratively


Photon mapping special parameters:

  -N[=BOOL], --photon-mapping-use-next-event-estimation[=BOOL]

  -E[=BOOL], --photon-mapping-exclusive-evaluation[=BOOL]

  -R[=BOOL], --photon-mapping-use-russian-roulette[=BOOL] 

  -r, --photon-mapping-evaluation-radius=REAL   Set the radius of the
                                                spherical limit where
                                                neighbor photons are
                                                evaluated.

  -e, --photon-mapping-evaluation-photons=INT   Set the number of neighbor
                                                photons to be evaluated.

  -t, --photon-mapping-total-saved-photons=INT  Set the total number of
                                                photons collisions within
                                                the scene.

Parallelization parameters:

  -D, --task-division=STRING       Array of pixels that compounds a task.

      Available divisions: region:WIDTH:HEIGTH, row, column, pixel

  -C, --task-concurrency=INT       Set the number of worker threads.
                                   By default, it uses CPU total
                                   number of logical threads.

      Special values:
        none       (1)  ->  No parallelization
        total      (0)  ->  Use all execution units

  -Q, --task-queue-size=INT        Set the number of entries of the
                                   task queue. Default size is 100.

)";

struct RawArguments
{
    using Arg = std::string_view;
    // Output image
    Arg dimensions;       // -d INT:INT
    Arg color_resolution; // -c INT | 8bit | 16bit | 32bit
    Arg scene_file;
    Arg destination_file;
    Arg output_format;    // -f STR

    // Parallelization
    Arg task_division;    // -D INT:INT | row | column | pixel
    Arg task_concurrency; // -C INT | total | none
    Arg task_queue_size;  // -Q INT
    
    // Render algorithm
    Arg algorithm; // -a path-tracing | pt | photon-mapping | pm

    // Path tracing parameters
    Arg paths_per_pixel;       // -p INT
    Arg path_tracing_strategy; // -s trace-projection | trace-direct-light | recursive | iterative
    
    // Photon mapping parameters
    Arg photon_mapping_use_next_event_estimation; // -N [BOOL]
    Arg photon_mapping_exclusive_evaluation;      // -E [BOOL]
    Arg photon_mapping_use_russian_roulette;      // -R [BOOL]
    Arg photon_mapping_evaluation_radius;         // -r REAL
    Arg photon_mapping_evaluation_photons;        // -e INT
    Arg photon_mapping_total_saved_photons;       // -t INT
};

struct Arguments
{
    // Output image
    Dimensions dimensions = {256, 256};
    Natural color_resolution = 255;
    std::string_view scene_file;
    std::string_view destination_file;
    std::string_view output_format;

    // Parallelization
    Dimensions task_division = {10, 10};
    Index task_concurrency = 0; // total
    Index task_queue_size = 100;
    
    // Render algorithm
    Algorithm algorithm = Algorithm::path_tracing;

    // Path tracing parameters
    Natural paths_per_pixel = 100; // Depends on algorithm: pt -> 100 / pm -> 10
    PathTracing::Strategy path_tracing_strategy;
    
    // Photon mapping parameters
    bool photon_mapping_use_next_event_estimation = false;
    bool photon_mapping_exclusive_evaluation = false;
    bool photon_mapping_use_russian_roulette = false;
    Real photon_mapping_evaluation_radius = 0.4;
    Index photon_mapping_evaluation_photons = 10'000; // all
    Index photon_mapping_total_saved_photons = 10'000;
};

Arguments processArgs(const RawArguments& raw)
{
    auto readNumber = []<typename Ty> (std::string_view num, Ty& value) 
    {
        Ty temp;
        const char* begin = num.data(), *end = num.data() + num.length();
        auto res = std::from_chars(begin, end, temp);
        if (res.ec == std::errc{}) {
            value = temp;
            return true;
        }
        return false;
    };

    auto checkDimensions = [readNumber](std::string_view str, Dimensions& dim) -> bool
    {
        auto getNum = [readNumber](std::string_view str, Index& value) -> std::pair<bool, Index>
        {
            auto dots = str.find_first_of(':');
            if (dots == std::string_view::npos)
                dots = str.length();
            auto num = str.substr(0, dots);
            if (num.empty())
                return {false, dots};
            return {readNumber(num, value), dots};
        };

        const auto[ok, dots] = getNum(str, dim.width);
        if (ok) return getNum({str.data() + dots + 1, str.size() - dots - 1}, dim.height).first;
        else    return false;
    };

    auto set = [](std::string_view arg) { return !arg.empty(); };

    auto oneOf = [](std::string_view arg, std::initializer_list<std::string_view> list)
    {
        for (auto s : list)
            if (arg == s)
                return true;
        return false;
    };

    Arguments args;
    args.scene_file = raw.scene_file;
    args.destination_file = raw.destination_file;
    args.output_format = raw.output_format;

    if (set(raw.dimensions) && !checkDimensions(raw.dimensions, args.dimensions))
        program::exit(program::err(), "Invalid image dimensions.");
    
    if (set(raw.color_resolution)) {
        if (oneOf(raw.color_resolution, {"8bit"}))
            args.color_resolution = std::numeric_limits<uint8_t>::max();
        else if (oneOf(raw.color_resolution, {"16bit"}))
            args.color_resolution = std::numeric_limits<uint16_t>::max();
        else if (oneOf(raw.color_resolution, {"32bit"}))
            args.color_resolution = std::numeric_limits<uint32_t>::max();
        else if (!readNumber(raw.color_resolution, args.color_resolution))
            program::exit(program::err(), "Invalid color resolution.");
    }

    if (set(raw.task_division)) {
        if (oneOf(raw.task_division, {"row"}))
            args.task_division = Dimensions{args.task_division.width, 1};
        else if (oneOf(raw.task_division, {"column"}))
            args.task_division = Dimensions{1, args.task_division.height};
        else if (oneOf(raw.task_division, {"pixel"}))
            args.task_division = Dimensions{1, 1};
        else if (!checkDimensions(raw.task_division, args.task_division))
            program::exit(program::err(), "Invalid task division.");
    }

    if (set(raw.task_concurrency)) {
        if (oneOf(raw.task_concurrency, {"total", "max"}))
            args.task_concurrency = 0;
        else if (oneOf(raw.task_division, {"none"}))
            args.task_concurrency = 1;
        else if (!readNumber(raw.task_concurrency, args.task_concurrency))
            program::exit(program::err(), "Invalid task concurrency.");
    }

    if (set(raw.task_queue_size) && !readNumber(raw.task_queue_size, args.task_queue_size))
        program::exit(program::err(), "Invalid queue size.");
       
    if (set(raw.algorithm)) {
        if (oneOf(raw.algorithm, {"path-tracing", "pt"}))
            args.algorithm = Algorithm::path_tracing;
        else if (oneOf(raw.algorithm, {"photon-mapping", "pm"}))
            args.algorithm = Algorithm::photon_mapping;
        else
            program::exit(program::err(), "Not supported algorithm.");
    }
    
    if (set(raw.paths_per_pixel)) {
        if (!readNumber(raw.paths_per_pixel, args.paths_per_pixel))
            program::exit(program::err(), "Invalid paths per pixel value.");
    } 
    else if (args.algorithm == Algorithm::photon_mapping)
        args.paths_per_pixel = 10; // Default value for photon mapping

    if (set(raw.path_tracing_strategy)) {
        if (raw.path_tracing_strategy == "trace-projection")
            args.path_tracing_strategy = PathTracing::Strategy::trace_projection;
        else if (raw.path_tracing_strategy == "trace-direct-light")
            args.path_tracing_strategy = PathTracing::Strategy::trace_direct_light;
        else if (raw.path_tracing_strategy == "recursive")
            args.path_tracing_strategy = PathTracing::Strategy::recursive;
        else if (raw.path_tracing_strategy == "iterative")
            args.path_tracing_strategy = PathTracing::Strategy::iterative;
        else
            program::exit(program::err(), "Not supported path tracing strategy.");
    }

    auto getBool = [set, oneOf](std::string_view str, bool& opt)
    {
        if (set(str)) {
            if (oneOf(str, {"true", "t", "1", "TRUE", "T"}))
                opt = true;
            else if (oneOf(str, {"false", "f", "0", "FALSE", "F"}))
                opt = false;
            else
                program::exit(program::err(), "Not a valid boolean: ", str);
        }
    };

    getBool(raw.photon_mapping_use_next_event_estimation,
            args.photon_mapping_use_next_event_estimation);

    getBool(raw.photon_mapping_exclusive_evaluation,
            args.photon_mapping_exclusive_evaluation);
    
    getBool(raw.photon_mapping_use_russian_roulette,
            args.photon_mapping_use_russian_roulette);

    if (set(raw.photon_mapping_evaluation_radius)
        && (!readNumber(raw.photon_mapping_evaluation_radius,
                        args.photon_mapping_evaluation_radius)
            || args.photon_mapping_evaluation_radius <= 0.0) )
    {
        program::exit(program::err(), "Invalid evaluation radius.");
    }

    if (set(raw.photon_mapping_evaluation_radius)
        && !readNumber(raw.photon_mapping_evaluation_radius,
                       args.photon_mapping_evaluation_radius))
    {
        program::exit(program::err(), "Invalid number of evaluation photons.");
    }
    
    if (set(raw.photon_mapping_total_saved_photons)
        && !readNumber(raw.photon_mapping_total_saved_photons,
                       args.photon_mapping_total_saved_photons))
    {
        program::exit(program::err(), "Invalid number of total saved photons.");
    }

    return args;
}

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

    auto set = [](std::string_view arg) { return !arg.empty(); };

    bool foundDst = false, foundSrc = false;

    RawArguments raw;
    for (int i = 1; i < argc; ++i)
    {
        Index pos;
        std::string_view str {argv[i]};

        auto parseOption = [&](std::string_view& opt,
                std::string_view duplicateError, std::string_view unspecifiedError)
        {
            if (set(opt))
                program::exit(program::err(), duplicateError, " defined more than once.");

            if (pos >= str.length())
            {
                if (++i == argc)
                    program::exit(program::err(), "No ", unspecifiedError, " specified.");
                opt = argv[i];
            }
            else { opt = str.substr(pos);}
        };

        auto parseBoolOption = [&](std::string_view& opt,
                std::string_view duplicateError, std::string_view unspecifiedError)
        {
            if (set(opt))
                program::exit(program::err(), duplicateError, " defined more than once.");

            if (pos < str.length())
            {
                if (str[pos] == '=') {
                    if (pos + 1 < str.length())
                        opt = str.substr(pos + 1);
                    else if (++i == argc)
                        program::exit(program::err(), "No ", unspecifiedError, " specified.");
                    else
                        opt = argv[i];
                }                    
                else {
                    program::exit(program::err(), "No ", unspecifiedError, " specified.");
                }
            }
            else
                opt = "true";
        };

        if (pos = checkOpt(str, "-h", "--help"); pos > 0)
        {
            program::exit(program::direct, helpStr);
        }
        else if (pos = checkOpt(str, "-d", "--dimensions="); pos > 0)
        {
            parseOption(raw.dimensions,
                    "Image dimensions", "dimensions");
        }
        else if (pos = checkOpt(str, "-c", "--color-resolution="); pos > 0)
        {
            parseOption(raw.color_resolution,
                    "Color resolution", "color resolution");
        }
        else if (pos = checkOpt(str, "-f", "--output-format="); pos > 0)
        {
            parseOption(raw.output_format,
                    "Output format", "format");
        }
        else if (pos = checkOpt(str, "-D", "--task-division="); pos > 0)
        {
            parseOption(raw.task_division,
                    "Task division", "task division");
        }
        else if (pos = checkOpt(str, "-C", "--task-concurrency="); pos > 0)
        {
            parseOption(raw.task_concurrency,
                    "Task concurrency", "task concurrency");
        }
        else if (pos = checkOpt(str, "-Q", "--task-queue-size="); pos > 0)
        {
            parseOption(raw.task_queue_size,
                    "Task queue size", "task queue size");
        }
        else if (pos = checkOpt(str, "-a", "--algorithm="); pos > 0)
        {
            parseOption(raw.algorithm,
                    "Render algorithm", "render algorithm");
        }
        else if (pos = checkOpt(str, "-p", "--paths-per-pixel="); pos > 0)
        {
            parseOption(raw.paths_per_pixel,
                    "Paths per pixel value", "paths per pixel");
        }
        else if (pos = checkOpt(str, "-s", "--path-tracing-strategy="); pos > 0)
        {
            parseOption(raw.path_tracing_strategy,
                    "Path tracing strategy", "path tracing strategy");
        }
        else if (pos = checkOpt(str, "-N", "--photon-mapping-use-next-event-estimation"); pos > 0)
        {
            parseBoolOption(raw.photon_mapping_use_next_event_estimation,
                    "Next event estimation flag", "next event estimation flag value");
        }
        else if (pos = checkOpt(str, "-E", "--photon-mapping-exclusive-evaluation"); pos > 0)
        {
            parseBoolOption(raw.photon_mapping_exclusive_evaluation,
                    "Exclusive evaluation flag", "exclusive evaluation flag value");
        }
        else if (pos = checkOpt(str, "-R", "--photon-mapping-use-russian-roulette"); pos > 0)
        {
            parseBoolOption(raw.photon_mapping_use_russian_roulette,
                    "Russian roulette flag", "russian roulette flag value");
        }
        else if (pos = checkOpt(str, "-r", "--photon-mapping-evaluation-radius="); pos > 0)
        {
            parseOption(raw.photon_mapping_evaluation_radius,
                    "Evaluation radius", "evaluation radius");
        }
        else if (pos = checkOpt(str, "-e", "--photon-mapping-evaluation-photons="); pos > 0)
        {
            parseOption(raw.photon_mapping_evaluation_photons,
                    "Number of evaluation photons", "number of evaluation photons");
        }
        else if (pos = checkOpt(str, "-t", "--photon-mapping-total-saved-photons="); pos > 0)
        {
            parseOption(raw.photon_mapping_total_saved_photons,
                    "Number of saved photons", "number saved photons");
        }
        else if (foundSrc && !foundDst) { raw.destination_file = str; foundDst = true; }
        else if (!foundSrc) { raw.scene_file = str; foundSrc = true; }
        else program::exit(program::err(), "Wrong number of arguments.");
    }

    if (!foundSrc)
        program::exit(program::err(), "No scene file specified.");

    if (!foundDst)
        program::exit(program::err(), "No destination file specified.");

    return processArgs(raw);
}

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
    const Arguments args = usage(argc, argv);

    // Read scene
    auto scene = [&args]() {
        auto s = makeSceneFromFile(args.scene_file);
        if (!s)
            program::exit(program::err(), "Could not read scene file or is incorrectly defined.");
        return s.value();
    }();

    Camera camera {scene.focus, scene.front, scene.up, args.dimensions};
    Image img {1, args.color_resolution, args.dimensions};

    auto writer = makeImageWriter(args.destination_file, args.output_format);
    if (!writer)
        program::exit(program::err(), "Could not open destination file or format not available.");
    
    auto render = [&](std::function<void(void)> renderFunc)
    {
        const auto seconds = measure(renderFunc);

        std::cout << "Render finished in " << seconds << " s\n";

        if (!writer->write(img))
            program::exit(program::err(), "Could not write destination file.");
    };

    switch (args.algorithm)
    {
    case Algorithm::photon_mapping:
        render([&]()
        {
            PhotonMapping::TaskDivider divider {args.dimensions, args.task_division};
            PhotonMapping::Renderer photonMapper {
                args.task_concurrency, args.task_queue_size, divider
            };

            photonMapper.render(camera, img, scene.objects,
                    args.paths_per_pixel, args.photon_mapping_total_saved_photons,
                    args.photon_mapping_evaluation_radius,
                    args.photon_mapping_evaluation_photons,
                    args.photon_mapping_use_next_event_estimation,
                    args.photon_mapping_exclusive_evaluation);
        });
        break;
    case Algorithm::path_tracing:
    default:
        render([&]()
        {
            PathTracing::TaskDivider divider {args.dimensions, args.task_division};
            PathTracing::Renderer pathTracer {
                args.task_concurrency, args.task_queue_size, divider,
                args.path_tracing_strategy
            };

            pathTracer.render(camera, img, scene.objects, args.paths_per_pixel);
        });
    }

    return 0;
}
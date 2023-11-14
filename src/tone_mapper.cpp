#include "image.hpp"
#include "image_reader.hpp"
#include "image_writer.hpp"
#include "tone_mapping.hpp"

#include "program.hpp"

static constexpr std::string_view helpStr = R"(
Usage: ./tone_mapper [OTPION...] INPUT_FILE OUTPUT_FILE
    
  -s, --strategy=STRING        Use this tone mapping strategy.
                               Equalizes if not specified.

      Available strategies:
        clamping:TOP                       (cl)
        equalization                       (eq)
        equalization_clamping:TOP          (eq_cl)
        gamma:GAMMA                        (gm)
        gamma_clamping:TOP:GAMMA           (gm_cl)

  -f, --output-format=STRING   Save image with this format. If not specified,
                               it is deduced from the file extension.
                            
      Available formats: ppm, bmp
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

    std::string_view source, destination, strategy, format;
    bool foundSrc = false;
    bool foundDst = false;
    bool foundStrategy = false;
    bool foundOutputFmt = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string_view str {argv[i]};
        if (Index pos = checkOpt(str, "-h", "--help"); pos > 0)
        {
            program::exit(program::direct, helpStr);
        }
        else if (Index pos = checkOpt(str, "-s", "--strategy="); pos > 0)
        {
            if (foundStrategy)
                program::exit(program::err(), "Strategy is defined more than once.");

            if (pos >= str.length())
            {
                if (++i == argc)
                    program::exit(program::err(), "No strategy specified.");
                strategy = argv[i];
            }
            else { strategy = str.substr(pos);}            

            foundStrategy = true;
        }
        else if (Index pos = checkOpt(str, "-f", "--output-format="); pos > 0)
        {
            if (foundOutputFmt)
                program::exit(program::err(), "Output format is defined more than once.");

            if (pos >= str.length())
            {
                if (++i == argc)
                    program::exit(program::err(), "No strategy specified.");
                format = argv[i];
            }
            else { format = str.substr(pos); } 

            foundOutputFmt = true;
        }
        else if (foundSrc) { destination = str; foundDst = true; }
        else if (!foundSrc) { source = str; foundSrc = true; }
        else program::exit(program::err(), "Wrong number of arguments.");
    }

    if (!foundSrc)
        program::exit(program::err(), "No source file specified.");

    if (!foundDst)
        program::exit(program::err(), "No destination file specified.");

    return std::tuple{source, destination, strategy, format};
}

int main(int argc, char* argv[])
{
    SET_PROGRAM_NAME(argv);
    auto [source, destination, strategy, format] = usage(argc, argv);

    auto reader = makeImageReader(source);
    if (reader == nullptr)
        program::exit(program::err(), "Could not open souce file.");
    
    Image img; 
    if (!reader->read(img))
        program::exit(program::err(), "Could not read any image from source file.");

    auto toneMaping = makeToneMappingStrategy(strategy);
    if (toneMaping == nullptr)
        program::exit(program::err(), "Strategy not supported.");

    auto writer = makeImageWriter(destination, format);
    if (writer == nullptr)
        program::exit(program::err(), "Could not open destination file or format not available.");

    img.toneMap(*toneMaping);
    img.changeResolution(255);

    if (!writer->write(img))
        program::exit(program::err(), "Could not write destination file.");

    return 0;
}
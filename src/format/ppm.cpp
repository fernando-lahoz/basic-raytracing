#include "format/ppm.hpp"
#include "image.hpp"
#include "numbers.hpp"

#include <ranges>
#include <charconv>
#include <concepts>

template<class Ty>
concept Readable = requires (char* ptr, Ty& n) { std::from_chars(ptr, ptr, n); };

void ppm::write(std::ostream& os, const Image& img)
{
    auto outputValue = [&](Real v) -> Natural
    {
        return std::round(v * (img.colorResolution / img.maxLuminance));
    };

    auto toString = [](Real x)
    {
        Index precision = FLT_DIG;
        if constexpr (std::is_same<Real, float>{}) precision = FLT_DIG;
        else if constexpr (std::is_same<Real, double>{}) precision = DBL_DIG;
        else if constexpr (std::is_same<Real, long double>{}) precision = LDBL_DIG;

        std::stringstream ss;
        ss << std::setprecision(precision) << x;
        return ss.str();
    };

    os << "P3\n";
    if (img.maxLuminance != 1)
        os << "#MAX=" << toString(img.maxLuminance) << '\n';
    os << std::to_string(img.nColumns) << ' ' << std::to_string(img.nRows) << '\n';
    os << std::to_string(img.colorResolution) << '\n';
    auto [width, height] = img.dimensions();
    for (Index i = 0; i < height; ++i)
    {   
        for (Index j = 0; j < width; ++j)
        {
            os << std::to_string(outputValue(img.redBuffer[i])) << ' ';
            os << std::to_string(outputValue(img.greenBuffer[i])) << ' ';
            os << std::to_string(outputValue(img.blueBuffer[i])) << ' ';
        }
        os << '\n';
    }
}

//Indulgent version
bool ppm::read(std::istream& is, Image& img)
{
    auto getline = [](std::istream& is, std::string& buff)
    {
        buff.clear();
        for(;;)
        {
            int c = is.get();
            if (c == '\r')
            {
                if (is.peek() == '\n')
                    is.get();
                return;
            }   
            else if (c == '\n') { return; }
            else if (c == std::istream::traits_type::eof()) { return; }
            else { buff.push_back(char(c)); } 
        }
    };

    auto checkEOL = [](std::string_view sv, std::size_t pos)
    {
        auto remaining = sv.find_first_not_of(" \t", pos);
        return (remaining == std::string_view::npos);
    };

    auto getNextNoEmptyLine = [&](std::stringstream& buffer)
    {   
        std::string line;
        do {
            do {
                getline(is, line);
                if (line.length() == 0 && !is)
                    return false;
            }
            while (line.length() == 0);
        }
        while (std::string{" \t"}.find(line[0]) != std::string::npos && checkEOL(line, 1));
        buffer = std::stringstream{line};
        return true;
    };

    std::stringstream buffer;
    bool error = false;

    auto readNumber = []<Readable Ty> (std::string_view num, Ty& value) 
    {
        Ty temp;
        const char* begin = num.data(), *end = num.data() + num.length();
        auto res = std::from_chars(begin, end, temp);
        if (res.ec == std::errc{})
        {
            value = temp;
            return true;
        }
        return false;
    };

    auto checkMAX = [&readNumber](std::string_view firstWord, std::stringstream& buffer, Real& value)
    {
        auto findValue = [&](std::string_view num)
        {
            if (num.length() > 0) return readNumber(num, value);
            else if (std::string str; buffer >> str) return readNumber(str, value);
            else return false;
        };

        auto findAsign = [&](std::string_view asign)
        {
            if (asign.length() > 0 && asign[0] == '=')
                { return findValue(asign.substr(1)); }
            else if (std::string next; buffer >> next && next[0] == '=')
                { return findValue(next.substr(1)); }
            
            return false;
        };

        if (firstWord.substr(1, 3) == "MAX")
            { return findAsign(firstWord.substr(4)); }
        else if (std::string secondWord; buffer >> secondWord
            && secondWord.substr(0, 3) == "MAX")
            { return findAsign(secondWord.substr(3)); }

        return false;
    };

    enum NextValue { WIDTH, HEIGHT, RESOLUTION, RED, GREEN, BLUE, END }; // order matters
    Index step = WIDTH;

    auto stateMachineNextStep = [&](std::string_view str)
    {
        auto inputValue = [&](Natural s) -> Real
            { return s * (img.maxLuminance / img.colorResolution); };

        static Index i = 0;
        Natural temp = 0;
        bool ok;

        switch (step++)
        {
        case WIDTH: return readNumber(str, img.nColumns);
        case HEIGHT: return readNumber(str, img.nRows);
        case RESOLUTION:
                    if (readNumber(str, img.colorResolution))
                    {
                        Index size = img.nColumns * img.nRows;
                        img.redBuffer.resize(size);
                        img.greenBuffer.resize(size);
                        img.blueBuffer.resize(size);
                        return true;
                    }
                    return false;
        case RED:   ok = readNumber(str, temp);
                    img.redBuffer[i] = inputValue(temp);
                    return ok;
        case GREEN: ok = readNumber(str, temp);
                    img.greenBuffer[i] = inputValue(temp);
                    return ok;
        case BLUE:
                    if (i < img.blueBuffer.size() - 1) step = RED;
                    else step = END;
                    ok = readNumber(str, temp);
                    img.blueBuffer[i++] = inputValue(temp);
                    return ok;
        default:    return false;
        }
    };

    auto processWords = [&]()
    {
        static bool foundMAX = false;
        std::string word;
        if (!(buffer >> word))
            return false;

        if (word[0] == '#')
        {
            error = (step >= RED);
            if (!foundMAX && !error) foundMAX = checkMAX(word, buffer, img.maxLuminance);
            return false;
        }
        error = !stateMachineNextStep(word);
        return !error;
    };

    while (step != END)
    {
        if (!getNextNoEmptyLine(buffer)) return false;
        while (processWords());
        if (error) return false;
    }

    return true;
}

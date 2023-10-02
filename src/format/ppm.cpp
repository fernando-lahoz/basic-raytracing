#include "format/ppm.hpp"
#include "image.hpp"
#include "numbers.hpp"

#include <ranges>
#include <charconv>


#include <iostream>

void ppm::write(std::ostream& os, const Image& img)
{
    auto outputValue = [&](Real v) -> Natural
    {
        return std::round(v * (img.colorResolution / img.maxLuminance));
    };

    auto toString = [](Real x)
    {
        std::size_t precision = FLT_DIG;
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
    for (auto i : std::views::iota(std::size_t{0}, img.nColumns * img.nRows))
    {
        os << std::to_string(outputValue(img.redBuffer[i])) << ' ';
        os << std::to_string(outputValue(img.greenBuffer[i])) << ' ';
        os << std::to_string(outputValue(img.blueBuffer[i])) << ' ';
    }
    os << '\n';
}

//Indulgent version
bool ppm::read(std::istream& is, Image& img)
{
    auto getline = [](std::istream& is, std::string& buff)
    {
        buff.clear();
        for(;;)
        {
            //std::cout << "buff:" << buff << std::endl;
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

    auto readNumber = [](std::string_view num, auto& value)
    {
        using Ty = typename std::remove_reference<decltype(value)>::type;
        Ty temp;
        auto res = std::from_chars(num.begin(), num.end(), temp);
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
            if (asign.length() > 0 && asign[0] == '=') return findValue(asign.substr(0));
            else if (std::string next; buffer >> next && next[0] == '=') return findValue(next);
            else return false;
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
        static Index i = 0;

        std::cout << "STEP: " << step << std::endl;

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
        case RED:   return readNumber(str, img.redBuffer[i]);
        case GREEN: return readNumber(str, img.greenBuffer[i]);
        case BLUE:
                    if (i < img.blueBuffer.size() - 1) step = RED;
                    else step = END;
                    return readNumber(str, img.blueBuffer[i++]);
        default:    return false;
        }
    };

    auto processWords = [&]()
    {
        static bool foundMAX = false;
        std::string word;
        if (!(buffer >> word))
            return false;

        std::cout << "WORD: " << word << std::endl;

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

        std::cout << "BUFFER: " << buffer.str() << std::endl;

        while (processWords());

        std::cout << "AFTER STEP: " << step << std::endl;
        if (error) return false;
    }

    return true;
}

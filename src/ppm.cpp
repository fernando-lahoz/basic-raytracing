#include "ppm.hpp"

//DEBUG
#include <iostream>

bool ppm::read(std::istream& is, ErrorMsg& error, Image& img)
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

    auto findNextWord = [](std::string_view sv, std::size_t pos, std::size_t& begin, std::size_t& end)
    {
        static constexpr std::size_t npos = std::string_view::npos;

        begin = sv.find_first_not_of(" \t", pos);
        if (begin == npos) { end = npos; return false; }

        end = sv.find_first_of(" \t", begin);
        if (end == npos) { end = sv.length(); return true; }
        
        return true;
    };

    auto getValue = [](std::string_view sv, std::size_t begin, std::size_t end, auto& value)
    {
        auto result = std::from_chars(sv.data() + begin, sv.data() + end, value);
        
        if constexpr (std::unsigned_integral<decltype(value)>)
            if (sv[begin] == '-')
                return false;

        return (result.ptr == sv.data() + end)
            && (result.ec == std::errc{});
    };

    auto checkEOL = [](std::string_view sv, std::size_t pos)
    {
        auto remaining = sv.find_first_not_of(" \t", pos);
        return (remaining == std::string_view::npos);
    };

    auto getNextNoEmptyLine = [&](std::string& buffer, bool& eof, bool& isComment)
    {   
        do {
            getline(is, buffer);

            if (buffer.length() == 0 && !is)
                { eof = true; isComment = false; return std::string_view{buffer}; }

            if (buffer[0] == '#')
                { eof = false; isComment = true; return std::string_view{buffer}; }
        }
        while (std::string{" \t"}.find(buffer[0]) != std::string::npos && checkEOL(buffer, 1) && is);
        
        eof = !bool(is); isComment = false;
        return std::string_view{buffer};
    };

    std::string buffer;
    std::string_view line;

    bool eof, isComment;
    std::size_t begin, end;

    bool foundMaxValue = false;

    // Check format
{
    do {
        line = getNextNoEmptyLine(buffer, eof, isComment); 
        if (eof) { error = "Early EOF found"; return false; }
    }
    while (isComment);
    
    if (!findNextWord(line, 0, begin, end) || line.substr(begin, end - begin) != "P3")
        { error = "Not supported format: " + buffer.substr(begin, end - begin); return false; }

    if (!checkEOL(line, end))
        { error = "Extra information following format was not expected: " + buffer.substr(end); return false; }
}
    
    // Look for #MAX=...
{
    line = getNextNoEmptyLine(buffer, eof, isComment); 
    if (eof) { error = "Early EOF found"; return false; }

    std::cout << "looking max:" << line << '\n';

    while (isComment)
    {
        if (foundMaxValue || !findNextWord(line, 1, begin, end))
            goto continue_loop;

        if (line.substr(begin, 3) == "MAX")
        {
            if (!findNextWord(line, begin + 3, begin, end)
                || line.substr(begin, end - begin)[0] != '=')
                { error = "An assignment was expected: " + buffer; return false; }

            if (!findNextWord(line, begin + 1, begin, end)
                || !(foundMaxValue = getValue(line, begin, end, img.maxValue)))
                { error = "A number was expected to be assigned: " + buffer; return false; }
            
            if (!checkEOL(line, end))
                { error = "Extra information following MAX assignment was not expected: " + buffer.substr(end); return false; }
        }

    continue_loop:
        line = getNextNoEmptyLine(buffer, eof, isComment);
        if (eof) { error = "Early EOF found"; return false; }
        std::cout << "looking max:" << line << '\n';
    }
}
    
    // Look for dimensions
{
    std::cout << "dimensions" << line << std::endl;

    if (!findNextWord(line, 0, begin, end) || !getValue(line, begin, end, img.nColumns))
        { error = "Width was expected: " + buffer.substr(begin, end - begin); return false; }

    if (!findNextWord(line, end, begin, end) || !getValue(line, begin, end, img.nRows))
        { error = "Height was expected: " + buffer.substr(begin, end - begin); return false; }

    if (!checkEOL(line, end))
        { error = "Extra information following image dimension was not expected: " + buffer.substr(end); return false; }

    if (img.nColumns == 0 || img.nRows == 0)
        { error = "Bad image dimension: " + buffer; return false; }
}

    // Look for color resolution
{
    do {
        line = getNextNoEmptyLine(buffer, eof, isComment); 
        if (eof) { error = "Early EOF found"; return false; }
    }
    while (isComment);

    std::cout << "color resolution" << line << std::endl;

    if (!findNextWord(line, 0, begin, end) || !getValue(line, begin, end, img.colorRes))
        { error = "Color space resolution was expected: " + buffer.substr(begin, end - begin); return false; }

    if (!checkEOL(line, end))
        { error = "Extra information following color space resolution was not expected: " + buffer.substr(end); return false; }
}

    // Fill the matrix
{
    do {
        line = getNextNoEmptyLine(buffer, eof, isComment); 
        if (eof) { error = "Early EOF found"; return false; }
    }
    while (isComment);

    std::stringstream ss {buffer};

    const std::size_t size = img.nColumns * img.nRows;

    img.redBuffer.resize(size);
    img.greenBuffer.resize(size);
    img.blueBuffer.resize(size);

    std::cout << "size: " << size << " \n";

    for (std::size_t i : std::views::iota(std::size_t{0}, size))
    {
        unsigned int red, green, blue; //DEPURAR POR AQUI

        if (!(ss >> buffer) && (!(is >> buffer)))
            { error = "Early EOF found"; return false; }
        
        std::cout << buffer << " ";

        if (!findNextWord(buffer, 0, begin, end) || !getValue(buffer, begin, end, red))
            { error = "Red value was expected: " + buffer.substr(begin, end - begin); return false; }

        if (!(ss >> buffer) && (!(is >> buffer)))
            { error = "Early EOF found"; return false; }

        std::cout << buffer << " ";

        if (!findNextWord(buffer, 0, begin, end) || !getValue(buffer, begin, end, green))
            { error = "Green value was expected: " + buffer.substr(begin, end - begin); return false; }

        if (!(ss >> buffer) && (!(is >> buffer)))
            { error = "Early EOF found"; return false; }
        
        std::cout << buffer << " \n";

        if (!findNextWord(buffer, 0, begin, end) || !getValue(buffer, begin, end, blue))
            { error = "Blue value was expected: " + buffer.substr(begin, end - begin); return false; }

        img.redBuffer[i] = red;
        img.greenBuffer[i] = green;
        img.blueBuffer[i] = blue;
    }
    
}
    do {
        getNextNoEmptyLine(buffer, eof, isComment); 
        if (eof) return true;
    }
    while (isComment);

    return true;
}
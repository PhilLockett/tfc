/**
 * @file    dump.cpp
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * 'tfc' is a command-line utility Text File Checker.
 *
 * Output file generator.
 */

#include <iostream>
#include <sstream>
#include <fstream>

#include "tfc.h"

namespace transform
{

/**
 * @section Internal constants and variables.
 *
 */

static bool isNewLine(char event)
{
    switch (event)
    {
    case '\n': 
    case '\r': return true;
    }
    return false;
}
#if 0
class State
{
public:
    int process(std::ostream &os, std::ifstream &is);

private:

    enum class Pos { start, begining, middle, end };
    Pos pos{Pos::start};
    char event;
    std::string processTab(void);
    std::string processSpace(void);
    std::string processLineFeed(void);
    std::string processCarriageReturn(void);
    std::string processAllOther(void);
};


std::string State::processTab(void)
{
    std::string ret{};
    switch (pos)
    {
    case Pos::start:
        pos = Pos::begining;    // fall through.

    case Pos::begining:
        if (Config::isSpace())
            ret = "    ";
        else
            ret = "\t";
        break;

    case Pos::middle:
            ret = "\t";
        break;

    case Pos::end:
        pos = Pos::begining;
            ret = "\t";
        break;
    }

    return ret;
}

std::string State::processSpace(void)
{
    std::string ret{};
    switch (pos)
    {
    case Pos::start:
        pos = Pos::begining;    // fall through.

    case Pos::begining:
        if (Config::isTab())
            ret = "\t";
        else
            ret = " ";
        break;

    case Pos::middle:
            ret = " ";
        break;

    case Pos::end:
        pos = Pos::begining;
            ret = " ";
        break;
    }

    return ret;
}

std::string State::processLineFeed(void)
{
    pos = Pos::end;

    return std::string{};
}

std::string State::processCarriageReturn(void)
{
    pos = Pos::end;

    return std::string{ "" };
}

std::string State::processAllOther(void)
{
    pos = Pos::middle;

    return std::string{ event };
}
int State::process(std::ostream &os, std::ifstream &is)
{
    std::string ret{};

    for (is.get(event); !is.eof(); is.get(event))
    {

        os << processChar(event) << processNewline(event);
    }

    return 0;
}
#endif
std::string padding(int column)
{
    if (Config::isTab())
    {
        const int size = Config::getTabSize();
        const int tabs = column / size;
        const int spaces = column - (tabs * size);
        return std::string(tabs, '\t') + std::string(spaces, ' ');
    }

    return std::string(column, ' ');
}


/**
 * @section whitespace handler.
 *
 */
std::string processChar(char event)
{
    enum class State { start, begining, middle, end };
    static State state{State::start};
    static int column{};
    const auto size{Config::getTabSize()};
    const auto ignore{!Config::isLeadingSet()};

    if (isNewLine(event))
    {
        std::string ret{};
        if (state == State::begining)
            ret = padding(column);

        state = State::end;

        return ret;
    }

    if (ignore)
    {
        return std::string{event};
    }

    switch (state)
    {
    case State::start:
        switch (event)
        {
        case ' ':
            state = State::begining;
            ++column;
            break;

        case '\t':
            state = State::begining;
            column = ((column / size) + 1) * size;
            break;

        default:
            state = State::middle;
            return std::string{event};
        }
    break;

    case State::begining:
        switch (event)
        {
        case ' ':
            ++column;
            break;

        case '\t':
            column = ((column / size) + 1) * size;
            break;

        default:
            state = State::middle;
            return padding(column) + std::string{event};
        }
    break;

    case State::middle:
        return std::string{event};
    break;

    case State::end:
        switch (event)
        {
        case ' ':
            state = State::begining;
            column = 1;
            break;

        case '\t':
            state = State::begining;
            column = size;
            break;

        default:
            state = State::middle;
            return std::string{event};
        }
    break;

    }

    return std::string{};
}


/**
 * @section newline handler.
 *
 */

static std::string processNewline(char event)
{
    enum class State { start, CR_rec, LF_rec, other };
    static State state{State::start};

    const auto ignore{!Config::isTrailingSet()};
    if ((ignore) && (isNewLine(event)))
        return std::string{event};

    const std::string dosNewline{ '\r', '\n' };
    const std::string unixNewline{ '\n' };
    const std::string newline{Config::isDos() ? dosNewline : unixNewline};

    switch (state)
    {
    case State::start:
        switch (event)
        {
        case '\r':  state = State::CR_rec;  break;
        case '\n':  state = State::LF_rec;  break;
        default:    state = State::other;
        }
    break;

    case State::CR_rec:
        switch (event)
        {
        case '\r':  break;

        case '\n':
        default:    state = State::other;
        }
        return newline;
    break;

    case State::LF_rec:
        switch (event)
        {
        case '\n':  break;

        case '\r':
        default:    state = State::other;
        }
        return newline;
    break;

    case State::other:
        switch (event)
        {
        case '\r':  state = State::CR_rec;  break;
        case '\n':  state = State::LF_rec;  break;
        default:    state = State::other;
        }
    }

    return std::string{};
}

/**
 * @section main code.
 *
 */

int processFile(std::ostream &os, std::ifstream &is)
{
    char event;

    for (is.get(event); !is.eof(); is.get(event))
    {

        os << processChar(event) << processNewline(event);
    }

    return 0;
}


/**
 * Process the user specified file.
 *
 * @return error value or 0 if no errors.
 */
int process(void)
{
    const std::string & inputFile{Config::getInputFile()};
//    State state{};

    std::ifstream is{inputFile, std::ios::binary};
    if (is.is_open()) 
    {
        if (std::ofstream os{Config::getOutputFile(), std::ios::binary})
        {
            processFile(os, is);
        }
        else
        {
            processFile(std::cout, is);
        }
    }
    else
    {
        std::cerr << "Unable to open file " << inputFile << '\n';
    }

    return 0;
}

} // namespace transform

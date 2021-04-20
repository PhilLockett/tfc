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

    return std::string{ "" };
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


/**
 * @section newline handler.
 *
 */

bool isNewline(char event)
{
    enum class State { start, CR_rec, LF_rec, other };
    static State state{State::start};

    bool newline = false;

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
        newline = true;
    break;

    case State::LF_rec:
        switch (event)
        {
        case '\n':  break;

        case '\r':
        default:    state = State::other;
        }
        newline = true;
    break;

    case State::other:
        switch (event)
        {
        case '\r':  state = State::CR_rec;  break;
        case '\n':  state = State::LF_rec;  break;
        default:    state = State::other;
        }
    }

    return newline;
}

/**
 * @section main code.
 *
 */

int State::process(std::ostream &os, std::ifstream &is)
{
    const std::string dosNewline{'\r', '\n' };
    const std::string unixNewline{'\n' };
    const std::string newline{Config::isDos() ? dosNewline : unixNewline};

    std::string ret{};

    for (is.get(event); !is.eof(); is.get(event))
    {
        switch (event)
        {
        case '\t':  ret = processTab();             break;
        case ' ':   ret = processSpace();           break;

        case '\n':  ret = processLineFeed();        break;
        case '\r':  ret = processCarriageReturn();  break;

        default:    ret = processAllOther();
        }

        if (isNewline(event))
            os << newline;
		os << ret;
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
    State state{};

    std::ifstream is{inputFile, std::ios::binary};
    if (is.is_open()) 
    {
        if (std::ofstream os{Config::getOutputFile(), std::ios::binary})
        {
            state.process(os, is);
        }
        else
        {
            state.process(std::cout, is);
        }
    }
    else
    {
        std::cerr << "Unable to open file " << inputFile << '\n';
    }

    return 0;
}

} // namespace transform

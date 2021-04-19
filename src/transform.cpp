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

struct State
{
    char event;
    bool start{true};
    bool space{};
    bool tab{};
    bool cr{};
    bool lf{};
    int lines{};
    int neither{};
    int spOnly{};
    int tabOnly{};
    int both{};
    int malformed{};
    int dos{};
    int unix{};
    void processTab(void) { if (start) tab = true; }
    void processSpace(void) { if (start) space = true; }
    void processLineFeed(void);
    void processCarriageReturn(void);
    void processAllOther(void);
    std::string swap(void);
    void displaySummary(std::ostream &os);
};

std::string State::swap(void)
{
    switch (event)
    {
    case '\t': return "▶ ";
    case ' ': return "⯀ ";
    case '\n': return "LF";
    case '\r': return "CR"; //"⮠";
    }
    return std::string{event, ' ', '\0'};
}

void State::displaySummary(std::ostream &os)
{
    os << "  Total Lines:\t" << lines << '\n';
    os << "Line begining:\n";
    if (spOnly)
        os << "  Space:\t" << spOnly << '\n';
    if (tabOnly)
        os << "  Tab:\t\t" << tabOnly << '\n';
    if (neither)
        os << "  Neither:\t" << neither << '\n';
    if (both)
        os << "  Both:\t\t" << both << '\n';
    os << "Line ending:\n";
    if (dos)
        os << "  Dos:\t\t" << dos << '\n';
    if (unix)
        os << "  Unix:\t\t" << unix << '\n';
    if (malformed)
        os << "  Malformed:\t" << malformed << '\n';
    os << '\n';
}

void State::processLineFeed(void)
{
    if (cr)
        ++dos;
    else
    {
        ++unix;
        lf = true;
    }

    ++lines;

    if (tab)
    {
        if (space)
            ++both;
        else
            ++tabOnly;
    }
    else
    {
        if (space)
            ++spOnly;
        else
            ++neither;
    }

    tab = false;
    space = false;
    start = true;
    cr = false;
}

void State::processCarriageReturn(void)
{
    if (lf)
        ++malformed;
    cr = true;
    lf = false;
}

void State::processAllOther(void)
{
    start = false;
    lf = false;
    cr = false;
}


/**
 * @section main code.
 *
 */

/**
 * Process the user specified file.
 *
 * @return error value or 0 if no errors.
 */
int process(void)
{
    const std::string & filename{Config::getInputFile()};
    std::cout << filename << '\n';
    State state{};

    if (std::ifstream is{filename, std::ios::binary | std::ios::ate})
    {
        is.seekg(0);
        while (is.read(&(state.event), 1))
        {
            switch (state.event)
            {
            case '\t':  state.processTab();             break;
            case ' ':   state.processSpace();           break;

            case '\n':  state.processLineFeed();        break;
            case '\r':  state.processCarriageReturn();  break;

            default:    state.processAllOther();
            }
        }
    }

    state.displaySummary(std::cout);

    return 0;
}

} // namespace transform

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


/**
 * @section Internal constants and variables.
 *
 */



/**
 * @section main code.
 *
 */
/*
2356 APL function
23CE return symbol
21b5
2BA0 Downward
*/

bool isWhiteSpace(char s)
{
    switch (s)
    {
    case '\t': 
    case ' ': return true;
    }
    return false;
}
bool isNewLine(char s)
{
    switch (s)
    {
    case '\n': 
    case '\r': return true;
    }
    return false;
}

std::string swap1(char s)
{
    switch (s)
    {
    case '\t': return "▶";
    case ' ': return "⯀";
    case '\n': return "⍖";
    case '\r': return "⏎"; //"⮠";
    }
    return std::string{s, '\0'};
}

std::string swap(char s)
{
    switch (s)
    {
    case '\t': return "▶ ";
    case ' ': return "⯀ ";
    case '\n': return "LF";
    case '\r': return "CR"; //"⮠";
    }
    return std::string{s, ' ', '\0'};
}

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
	void displaySummary(std::ostream &os);

};

void State::displaySummary(std::ostream &os)
{
    os << '\n';
    os << "Lines:\t" << lines << '\n';
    if (dos)
        os << "Dos:\t" << dos << '\n';
    if (unix)
        os << "Unix:\t" << unix << '\n';
    if (neither)
        os << "Neither:\t" << neither << '\n';
    if (spOnly)
        os << "Space:\t" << spOnly << '\n';
    if (tabOnly)
        os << "Tab:\t" << tabOnly << '\n';
    if (both)
        os << "Both:\t" << both << '\n';
    if (malformed)
        os << "Malformed:\t" << malformed << '\n';
    os << '\n';
}

static int go(std::string filename)
{
    std::cout << filename << '\n';
    State state{};

    if (std::ifstream is{filename, std::ios::binary | std::ios::ate})
    {
        auto size = is.tellg();
        is.seekg(0);
        while (is.read(&(state.event), 1))
        {
            switch (state.event)
            {
            case '\t':
                if (state.start)
                    state.tab = true;
                break;

            case ' ':
                if (state.start)
                    state.space = true;
                break;

            case '\n':
                if (state.cr)
                    ++(state.dos);
                else
                {
                    ++(state.unix);
                    state.lf = true;
                }
                ++(state.lines);
                if (state.tab)
                {
                    if (state.space)
                        ++(state.both);
                    else
                        ++(state.tabOnly);
                }
                else
                {
                    if (state.space)
                        ++(state.spOnly);
                    else
                        ++(state.neither);
                }
                state.tab = false;
                state.space = false;
                state.start = true;
                state.cr = false;
                break;

            case '\r':
                if (state.lf)
                    ++(state.malformed);
                state.cr = true;
                state.lf = false;
                break;

            default:
                state.start = false;
                state.lf = false;
                state.cr = false;
            }

            // std::cout << swap(state.e) << ' ' << (int)(state.e) << '\n';
            std::cout << swap(state.event);

            if (state.event == '\n')
                std::cout << '\n';
        }
    }

    state.displaySummary(std::cout);

    return 0;
}

/**
 * The bulk of the script generation work.
 *
 * @return error value or 0 if no errors.
 */
int process(void)
{
#if 0
    std::string filenameCRLF = "TestCRLF.txt";
    std::string filenameLF = "TestLF.txt";

    go(filenameCRLF);
    go(filenameLF);
#else
    go(Config::getInputFile());
#endif
    return 0;
}

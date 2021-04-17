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

int go(std::string filename)
{
    std::cout << filename << '\n';
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

    if (std::ifstream is{filename, std::ios::binary | std::ios::ate})
    {
        char b;
        auto size = is.tellg();
        is.seekg(0);
        while (is.read(&b, 1))
        {
            switch (b)
            {
            case '\t':
                if (start)
                    tab = true;
                break;

            case ' ':
                if (start)
                    space = true;
                break;

            case '\n':
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
                break;

            case '\r':
                if (lf)
                    ++malformed;
                cr = true;
                lf = false;
                break;

            default:
                start = false;
                lf = false;
                cr = false;

                // tab = false;
                // space = false;
            }

            // std::cout << swap(b) << ' ' << (int)(b) << '\n';
            std::cout << swap(b);

            if (b == '\n')
                std::cout << '\n';
        }
    }

    std::cout << '\n';
    std::cout << "Lines:\t" << lines << '\n';
    if (dos)
        std::cout << "Dos:\t" << dos << '\n';
    if (unix)
        std::cout << "Unix:\t" << unix << '\n';
    if (neither)
        std::cout << "Neither:\t" << neither << '\n';
    if (spOnly)
        std::cout << "Space:\t" << spOnly << '\n';
    if (tabOnly)
        std::cout << "Tab:\t" << tabOnly << '\n';
    if (both)
        std::cout << "Both:\t" << both << '\n';
    if (malformed)
        std::cout << "Malformed:\t" << malformed << '\n';
    std::cout << '\n';

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

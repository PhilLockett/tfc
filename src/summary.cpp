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

class State
{
public:
    int process(std::ostream &os, std::ifstream &is);

private:
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
    void processTab(void);
    void processSpace(void);
    void processLineFeed(void);
    void processCarriageReturn(void);
    void processAllOther(void);
    void display(std::ostream &os);
    void displaySummary(std::ostream &os);
    void displayDebug(std::ostream &os);
};

void State::displaySummary(std::ostream &os)
{
    os << Config::getInputFile().string() << '\n';
    os << "  Total Lines:\t" << lines << '\n';
    os << "Line begining:\n";
    if (spOnly)
        os << "  Space only:\t" << spOnly << '\n';
    if (tabOnly)
        os << "  Tab only:\t" << tabOnly << '\n';
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

void State::displayDebug(std::ostream &os)
{
    os << Config::getInputFile().string() << '\n';
    os << lines;
    os << " " << spOnly;
    os << " " << tabOnly;
    os << " " << neither;
    os << " " << both;
    os << " " << dos;
    os << " " << unix;
    os << " " << malformed;
    os << '\n';
}

void State::display(std::ostream &os)
{
    if (Config::isDebug())
        displayDebug(os);
    else
        displaySummary(os);
}

void State::processTab(void)
{
    if (start)
        tab = true;

    lf = false;
    cr = false;
}

void State::processSpace(void)
{
    if (start)
        space = true;

    lf = false;
    cr = false;
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
    {
        ++malformed;
        --unix;		// Was counted as unix, but shouldn't be.
        cr = false;
    }
    else
        cr = true;

    lf = false;
}

void State::processAllOther(void)
{
    start = false;
    lf = false;
    cr = false;
}


int State::process(std::ostream &os, std::ifstream &is)
{
    for (is.get(event); !is.eof(); is.get(event))
    {
        switch (event)
        {
        case '\t':  processTab();               break;
        case ' ':   processSpace();             break;
    
        case '\n':  processLineFeed();          break;
        case '\r':  processCarriageReturn();    break;
    
        default:    processAllOther();
        }
    }

    display(os);

    return 0;
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
int processSummary(void)
{
    State state{};
    const auto & inputFile{Config::getInputFile()};

    std::ifstream is{inputFile, std::ios::binary};
    if (is.is_open()) 
    {
        if (std::ofstream os{Config::getOutputFile(), std::ios::out})
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

        return 1;
    }

    return 0;
}


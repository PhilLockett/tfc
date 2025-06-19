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
#include "configuration.h"



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
    int dosEOL{};
    int unixEOL{};
    int ansi{};
    int utf8{};
    int utf8Needed{};
    int utf8Processed{};
    int utf8Error{};
    void processTab(void);
    void processSpace(void);
    void processLineFeed(void);
    void processCarriageReturn(void);
    void processAllOther(unsigned char event);
    void display(std::ostream &os);
    void displaySummary(std::ostream &os);
    void displayDebug(std::ostream &os);
};

static void streamValue(std::ostream &os, const std::string & label, int value)
{
    if (value == 0)
    {
        return;
    }

    os << "  ";

	std::string valStr{std::to_string(value)};
    std::string padding(18 - label.length() - valStr.length(), ' ');
    os << label << ":" << padding << valStr;

    os << "\n";
}

void State::displaySummary(std::ostream &os)
{
    os << Config::getInputFile().string() << '\n';
    streamValue(os, "Total Lines", lines);

    os << "Line beginning:\n";
    streamValue(os, "Space only", spOnly);
    streamValue(os, "Tab only", tabOnly);
    streamValue(os, "Neither", neither);
    streamValue(os, "Both", both);

    os << "Line ending:\n";
    streamValue(os, "Dos", dosEOL);
    streamValue(os, "Unix", unixEOL);
    streamValue(os, "Malformed", malformed);

    if (ansi || utf8)
    {
        os << "Character encoding:\n";
        streamValue(os, "ANSI", ansi);
        streamValue(os, "UTF-8", utf8);
    }

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
    os << " " << dosEOL;
    os << " " << unixEOL;
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
        ++dosEOL;
    else
    {
        ++unixEOL;
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
        --unixEOL;		// Was counted as unix, but shouldn't be.
        cr = false;
    }
    else
        cr = true;

    lf = false;
}

static int numUtf8Bytes(unsigned char lead)
{
    if ((lead & 0x80) == 0x0)   return 1;
    if ((lead & 0xE0) == 0xC0)  return 2;
    if ((lead & 0xF0) == 0xE0)  return 3;
    if ((lead & 0xF8) == 0xF0)  return 4;

    return 0;
}

static bool isUtf8ContinuationByte(unsigned char byte)
{
    return ((byte & 0xC0) == 0x80);
}

void State::processAllOther(unsigned char event)
{
    start = false;
    lf = false;
    cr = false;

    if (event > 0x7F)
    {
        if (utf8Needed)
        {
            if (isUtf8ContinuationByte(event))
            {
                ++utf8Processed;
                if (utf8Processed == utf8Needed)
                {
                    ++utf8;
                    utf8Needed = 0;
                    utf8Processed = 0;
                }


            }
            else
            {
                ansi += utf8Processed;
                utf8Needed = 0;
                utf8Processed = 0;
            }

        }
        else
        {
            int length{numUtf8Bytes(event)};
            if (length)
            {
                utf8Needed = length;
                utf8Processed = 1;
            }
            else
            {
                ++ansi;
            }
        }

    }
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
    
        default:    processAllOther(event);
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


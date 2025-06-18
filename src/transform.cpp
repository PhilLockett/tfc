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


class Status
{
public:
    Status(void) :
        size{Config::getTabSize()},
        ignoreHead{!Config::isLeadingSet()},
        ignoreTail{!Config::isTrailingSet()},
        space{Config::isSpace()},
        tab{Config::isTab()},
        newline{Config::isDos() ? std::string("\r\n") : std::string("\n") },
        state{State::start},
        nlState{NLState::start},
        event{},
        column{}
        {}
    int process(std::ostream &os, std::ifstream &is);

private:
    enum class State { start, begining, middle, end };
    enum class NLState { start, CR_rec, LF_rec, other };

    const size_t size;
    const bool ignoreHead;
    const bool ignoreTail;
    const bool space;
    const bool tab;
    const std::string newline;

    State state;
    NLState nlState;
    char event;
    int column;
    
    bool isNewLine(void) const {    return ((event == '\n') || (event == '\r')); }
    std::string padding(void);
    std::string processChar(void);
    std::string processNewline(void);
};

/**
 * @section Utility functions.
 *
 */

std::string Status::padding(void)
{
    if (tab)
    {
        const int tabs = column / size;
        const int spaces = column - (tabs * size);

        return std::string(tabs, '\t') + std::string(spaces, ' ');
    }

    if (space)
    {
        return std::string(column, ' ');
    }

    return std::string{};
}


/**
 * @section whitespace handler.
 *
 */
 
std::string Status::processChar(void)
{
    if (ignoreHead)
    {
        if (isNewLine())
        {
            state = State::end;
            return std::string{};
        }

        return std::string{event};
    }

    if (isNewLine())
    {
        std::string ret{};
        if (state == State::begining)
            ret = padding();

        state = State::end;

        return ret;
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
            return padding() + std::string{event};
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

std::string Status::processNewline(void)
{
    if ((ignoreTail) && (isNewLine()))
        return std::string{event};


    switch (nlState)
    {
    case NLState::start:
        switch (event)
        {
        case '\r':
            nlState = NLState::CR_rec;
            return newline;

        case '\n':
            nlState = NLState::LF_rec;
            return newline;

        default:
            nlState = NLState::other;
        }
    break;

    case NLState::CR_rec:
        switch (event)
        {
        case '\r':
            return newline;

        case '\n':
            nlState = NLState::other;
            break;

        default:
            nlState = NLState::other;
        }
    break;

    case NLState::LF_rec:
        switch (event)
        {
        case '\r':
            nlState = NLState::other;
            break;

        case '\n':
            return newline;

        default:
            nlState = NLState::other;
        }
    break;

    case NLState::other:
        switch (event)
        {
        case '\r':
            nlState = NLState::CR_rec;
            return newline;

        case '\n':
            nlState = NLState::LF_rec;
            return newline;

        default:
            nlState = NLState::other;
        }
    }

    return std::string{};
}


int Status::process(std::ostream &os, std::ifstream &is)
{
    for (is.get(event); !is.eof(); is.get(event))
    {
        os << processChar() << processNewline();
    }

    return 0;
}


/*****************************************************************************
 * @section main code.
 *
 */

/**
 * Process the user specified file.
 *
 * @return error value or 0 if no errors.
 */
int processTransform(void)
{
    Status state{};
    const auto & inputFile{Config::getInputFile()};

    std::ifstream is{inputFile, std::ios::binary};
    if (is.is_open()) 
    {
        if (Config::isReplacing())
        {
            auto tempFile{std::filesystem::temp_directory_path()};
            tempFile /= inputFile.filename();  // Temporary file path.
            if (std::ofstream os{tempFile, std::ios::binary})
            {
                state.process(os, is);
                os.close();
                is.close();

                const auto copyOptions = std::filesystem::copy_options::overwrite_existing;
                if (!std::filesystem::copy_file(tempFile, inputFile, copyOptions))   // Overwrite.
                {
                    std::cerr << "Failed to copy file " << tempFile << " to " << inputFile << '\n';
                }
                std::filesystem::remove(tempFile);   // Clean up.
            }
            else
            {
                std::cerr << "Failed to open file " << tempFile << '\n';

                return 1;
            }
        }
        else if (std::ofstream os{Config::getOutputFile(), std::ios::binary})
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


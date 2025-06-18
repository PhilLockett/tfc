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
    enum class State { start, beginning, middle, end };
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

    std::string processCharStart(void);
    std::string processCharBeginning(void);
    std::string processCharEnd(void);
    std::string processChar(void);

    std::string processNewlineStart(void);
    std::string processNewlineCR(void);
    std::string processNewlineLF(void);
    std::string processNewlineOther(void);
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
std::string Status::processCharStart(void)
{
    switch (event)
    {
    case ' ':
        state = State::beginning;
        ++column;
        break;

    case '\t':
        state = State::beginning;
        column = ((column / size) + 1) * size;
        break;

    default:
        state = State::middle;
        return std::string{event};
    }

    return "";
}

std::string Status::processCharBeginning(void)
{
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

    return "";
}

std::string Status::processCharEnd(void)
{
    switch (event)
    {
    case ' ':
        state = State::beginning;
        column = 1;
        break;

    case '\t':
        state = State::beginning;
        column = size;
        break;

    default:
        state = State::middle;
        return std::string{event};
    }

    return "";
}

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
        if (state == State::beginning)
            ret = padding();

        state = State::end;

        return ret;
    }

    std::string result{};
    switch (state)
    {
    case State::start:
        result = processCharStart();
        if (!result.empty())
        {
            return result;
        }
    break;

    case State::beginning:
        result = processCharBeginning();
        if (!result.empty())
        {
            return result;
        }
    break;

    case State::middle:
        return std::string{event};
    break;

    case State::end:
        result = processCharEnd();
        if (!result.empty())
        {
            return result;
        }
    break;

    }

    return std::string{};
}


/**
 * @section newline handler.
 *
 */

std::string Status::processNewlineStart(void)
{
    switch (event)
    {
    case '\r':  nlState = NLState::CR_rec;  return newline;
    case '\n':  nlState = NLState::LF_rec;  return newline;

    default:    nlState = NLState::other;
    }

    return "";
}

std::string Status::processNewlineCR(void)
{
    switch (event)
    {
    case '\r':  return newline;
    case '\n':  nlState = NLState::other;   break;

    default:    nlState = NLState::other;
    }

    return "";
}

std::string Status::processNewlineLF(void)
{
    switch (event)
    {
    case '\r':  nlState = NLState::other;   break;
    case '\n':  return newline;

    default:    nlState = NLState::other;
    }

    return "";
}

std::string Status::processNewlineOther(void)
{
    switch (event)
    {
    case '\r':  nlState = NLState::CR_rec;  return newline;
    case '\n':  nlState = NLState::LF_rec;  return newline;

    default:    nlState = NLState::other;
    }

    return "";
}

std::string Status::processNewline(void)
{
    if ((ignoreTail) && (isNewLine()))
        return std::string{event};

    std::string result{};
    switch (nlState)
    {
    case NLState::start:
        result = processNewlineStart();
        if (!result.empty())
        {
            return result;
        }
    break;

    case NLState::CR_rec:
        result = processNewlineCR();
        if (!result.empty())
        {
            return result;
        }
    break;

    case NLState::LF_rec:
        result = processNewlineLF();
        if (!result.empty())
        {
            return result;
        }
    break;

    case NLState::other:
        result = processNewlineOther();
        if (!result.empty())
        {
            return result;
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


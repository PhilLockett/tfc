/**
 * @file    configuration.cpp
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
 * Implementation of the tfc configuration Singleton.
 */

#include "configuration.h"


/**
 * @section tfc configuration Singleton implementation.
 */

void Config::display(std::ostream &os) const
{
    os << "Config is " << std::string{Config::isValid() ? "" : "NOT "} << "valid\n";
    os << "Input file name:  " << Config::getInputFile() << '\n';
    os << "Output file name: " << Config::getOutputFile() << '\n';
    if (Config::isLeadingSet())
    {
        if (Config::isSpace())
            os << "Leading spaces will be replaced with tabs\n";
        else
            os << "Leading tabs will be replaced with spaces\n";
    }
    else
    {
        os << "Leading whitespace will be unchanged\n";
    }
    if (Config::isTrailingSet())
    {
        if (Config::isDos())
            os << "Newlines will be DOS style\n";
        else
            os << "Newlines will be Unix style\n";
    }
    else
    {
        os << "Newlines will be unchanged\n";
    }
    os << "Tab size: " << Config::getTabSize() << '\n';
    if (Config::isReplacing())
        os << "Overwriting source file contents.\n";
    if (Config::isDebug())
        os << "Generating debug summary.\n";
}


bool Config::isValid(bool showErrors)
{
    namespace fs = std::filesystem;

    const auto & inputFile{Config::getInputFile()};

    if (inputFile.string().empty())
    {
        if (showErrors)
            std::cerr << "\nInput (or replacement) file must be specified.\n";

        return false;
    }

    if (!fs::exists(inputFile))
    {
        if (showErrors)
        {
            std::cerr << "\nInput file " << inputFile << " does not exist.\n";
        }

        return false;
    }

    if (Config::isReplacing() && Config::isSummary())
    {
        if (showErrors)
        {
            std::cerr << "\nCannot overwrite input file " << inputFile << " with a summary.\n";
        }

        return false;
    }

    const auto & outputFile{Config::getOutputFile()};

    if (fs::exists(outputFile))
    {
        if (fs::equivalent(inputFile, outputFile))
        {
            if (showErrors)
            {
                std::cerr << "\nInput and output files are the same. To replace source file use:\n";
                std::cerr << "\ttfc -r " << inputFile.string() << " [Options]\n";
            }
        
            return false;
        }

        if (showErrors)
        {
            std::cerr << "\nWarning output file " << outputFile << " will be overwritten.\n";
        }
    }

    return true;
}


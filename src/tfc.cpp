/**
 * @file    tfc.cpp
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
 * System entry point for the text file checker.
 */

#include "tfc.h"


/**
 * @section tfc configuration data.
 *
 * Implementation of the tfc comand line configuration Singleton.
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
}


/**
 * @section System entry point.
 *
 */

/**
 * main.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
//- Get the command line parameters.
    if (init(argc, argv) < 0)
    {
        return 1;
    }

#if 0
    std::cout << Config::get() << '\n';
#endif


//- If all is well, generate the file.
    if (Config::isValid())
    {
        process();
    }

    return 0;
}


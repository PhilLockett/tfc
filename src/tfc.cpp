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
#include "configuration.h"


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
//- Process the command line parameters.
    auto i = Config::instance().init(argc, argv);
    if (i < 0)
    {
        return 1;
    }
    else if (i > 0)
    {
        return 0;
    }

#if 0
    std::cout << Config::instance() << '\n';
#endif

    if (!Config::isValid(true))
    {
        return 1;
    }

//- If all is well, generate the output.
    if (Config::isChangeRequested())
    {
        return processTransform();
    }
    else
    {
        return processSummary();
    }

    return 0;
}

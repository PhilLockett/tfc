/**
 * @file    init.cpp
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
 * Command line parameter initialisation for the text file checker.
 */

#include <getopt.h>
#include <future>

#include "tfc.h"
#include "config.h"



/**
 * Display version message.
 *
 * @param  name - of application.
 */
static auto version(const char * const name)
{
    std::cout << "Version " VERSION " of " << name << '\n';
    std::cout << "Feedback to: " PACKAGE_BUGREPORT << '\n';

    return 2;
}


/**
 * Display help message.
 *
 * @param  name - of application.
 */
static auto help(const char * const name)
{
    std::cout << "Usage: " << name << " [Options]\n";
    std::cout << '\n';
    std::cout << "  Analyzes the given file for consistent leading whitespace and line endings\n";
    std::cout << '\n';
    std::cout << "  Options:\n";
    std::cout << "\t-h --help \t\tThis help page and nothing else.\n";
    std::cout << "\t-v --version \t\tDisplay version.\n";
    std::cout << "\t-i --input <file> \tInput file name.\n";
    std::cout << "\t-o --output <file> \tOutput file name (default: console).\n";
    std::cout << "\t-d --dos\t\tDOS style End-Of-line.\n";
    std::cout << "\t-u --unix\t\tUnix style End-Of-line.\n";
    std::cout << "\t-s --space\t\tUse leading spaces.\n";
    std::cout << "\t-t --tab\t\tUse leading tabs.\n";

    return 1;
}


/**
 * Process command line parameters with help from getopt_long() and update
 * global variables.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int parseCommandLine(int argc, char *argv[], Config &config)
{
    while (1)
    {
        int option_index = 0;
        int optchr;

        static struct option long_options[] = {
            {"help",    no_argument,0,'h'},
            {"version", no_argument,0,'v'},
            {"input",   required_argument,0,'i'},
            {"output",  required_argument,0,'o'},
            {"dos",     no_argument,0,'d'},
            {"unix",    no_argument,0,'u'},
            {"space",   no_argument,0,'s'},
            {"tab",     no_argument,0,'t'},
            {0,0,0,0}
        };

        optchr = getopt_long(argc, argv ,"hvi:o:dust", long_options, &option_index);
        if (optchr == -1)
            return 0;

        switch (optchr)
        {
            case 'h': return help(argv[0]);
            case 'v': return version(argv[0]);

            case 'i': config.setInputFile(std::string(optarg)); break;
            case 'o': config.setOutputFile(std::string(optarg)); break;

            case 'd': config.setDos();     break;
            case 'u': config.setUnix();    break;

            case 's': config.setSpaces();  break;
            case 't': config.setTabs();    break;

            default:
                help(argv[0]);

                return -1;
        }//end switch
    }//end while

    return 0;
}



/**
 * Initialise 'tfc' using command line input and ensure we only do it once.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int init(int argc, char *argv[])
{
    static std::once_flag virgin;
    int ret = 0;

    std::call_once(virgin, [&](){ ret = parseCommandLine(argc, argv, Config::get()); });

    return ret;
}


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

#include <getopt.h>
#include <future>

#include "config.h"
#include "configuration.h"


/**
 * @section tfc configuration Singleton implementation.
 *
 * Command line parameter initialisation.
 */

/**
 * Display version message.
 *
 * @param  name - of application.
 */
int Config::version(const char * const name)
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
int Config::help(const char * const name)
{
    std::cout << "Usage: " << name << " [Options]\n";
    std::cout << '\n';
    std::cout << "  Analyzes the given file for consistent leading whitespace and line endings.\n";
    std::cout << "  Corrects leading whitespace and line endings as required.\n";
    std::cout << '\n';
    std::cout << "  Options:\n";
    std::cout << "\t-h --help \t\tThis help page and nothing else.\n";
    std::cout << "\t-v --version \t\tDisplay version.\n";
    std::cout << "\t-i --input <file> \tInput file name.\n";
    std::cout << "\t-o --output <file> \tOutput file name (default: console).\n";
    std::cout << "\t-r --replace <file> \tReplace file with transformed version.\n";
    std::cout << "\t-d --dos\t\tDOS style End-Of-line.\n";
    std::cout << "\t-u --unix\t\tUnix style End-Of-line.\n";
    std::cout << "\t-s --space\t\tUse leading spaces.\n";
    std::cout << "\t-t --tab\t\tUse leading tabs.\n";
    std::cout << "\t-2 \t\t\tSet tab size to 2 spaces.\n";
    std::cout << "\t-4 \t\t\tSet tab size to 4 spaces (default).\n";
    std::cout << "\t-8 \t\t\tSet tab size to 8 spaces.\n";

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
int Config::parseCommandLine(int argc, char *argv[])
{
    if (argc < 2)
    {
        help(argv[0]);

        return -1;
    }

    while (1)
    {
        int option_index = 0;
        int optchr;

        static struct option long_options[] = {
            {"help",    no_argument,0,'h'},
            {"version", no_argument,0,'v'},
            {"input",   required_argument,0,'i'},
            {"output",  required_argument,0,'o'},
            {"replace", required_argument,0,'r'},
            {"dos",     no_argument,0,'d'},
            {"unix",    no_argument,0,'u'},
            {"space",   no_argument,0,'s'},
            {"tab",     no_argument,0,'t'},
            {0,0,0,0}
        };

        optchr = getopt_long(argc, argv ,"hvi:o:r:dust248x", long_options, &option_index);
        if (optchr == -1)
            return 0;

        switch (optchr)
        {
            case 'h': return help(argv[0]);
            case 'v': return version(argv[0]);

            case 'i': setInputFile(std::string(optarg)); break;
            case 'o': setOutputFile(std::string(optarg)); break;
            case 'r': setReplaceFile(std::string(optarg)); break;

            case 'd': setDos();     break;
            case 'u': setUnix();    break;

            case 's': setSpaces();  break;
            case 't': setTabs();    break;

            case '2': setTabSize(2);break;
            case '4': setTabSize(4);break;
            case '8': setTabSize(8);break;

            case 'x': enableDebug();break;

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
int Config::init(int argc, char *argv[])
{
    static std::once_flag virgin;
    int ret = 0;

    std::call_once(virgin, [&](){ ret = parseCommandLine(argc, argv); });

    return ret;
}


/**
 * @section tfc configuration Singleton implementation.
 *
 * Support code.
 */

void Config::display(std::ostream &os) const
{
    os << "Config is " << std::string{isValid() ? "" : "NOT "} << "valid\n";
    os << "Input file name:  " << getInputFile() << '\n';
    os << "Output file name: " << getOutputFile() << '\n';
    if (isLeadingSet())
    {
        if (isSpace())
            os << "Leading spaces will be replaced with tabs\n";
        else
            os << "Leading tabs will be replaced with spaces\n";
    }
    else
    {
        os << "Leading whitespace will be unchanged\n";
    }
    if (isTrailingSet())
    {
        if (isDos())
            os << "Newlines will be DOS style\n";
        else
            os << "Newlines will be Unix style\n";
    }
    else
    {
        os << "Newlines will be unchanged\n";
    }
    os << "Tab size: " << getTabSize() << '\n';
    if (isReplacing())
        os << "Overwriting source file contents.\n";
    if (isDebug())
        os << "Generating debug summary.\n";
}


bool Config::isValid(bool showErrors)
{
    namespace fs = std::filesystem;

    const auto & inputFile{getInputFile()};

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

    if (isReplacing() && isSummary())
    {
        if (showErrors)
        {
            std::cerr << "\nCannot overwrite input file " << inputFile << " with a summary.\n";
        }

        return false;
    }

    const auto & outputFile{getOutputFile()};

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


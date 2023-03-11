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

#include <future>
#include <vector>

#include "configuration.h"
#include "Opts.h"


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
int Config::version(void)
{
    std::cout << "Version " VERSION " of " << name << '\n';
    std::cout << "Feedback to: " PACKAGE_BUGREPORT << '\n';

    return 2;
}


const Opts::OptsType optList
{
    { 'h', "help",    NULL,   "This help page and nothing else." },
    { 'v', "version", NULL,   "Display version." },
    { 'i', "input",   "file", "Input file name." },
    { 'o', "output",  "file", "Output file name (default: console)." },
    { 'r', "replace", "file", "Replace file with transformed version." },
    { 'd', "dos",     NULL,   "DOS style End-Of-line." },
    { 'u', "unix",    NULL,   "Unix style End-Of-line." },
    { 's', "space",   NULL,   "Use leading spaces." },
    { 't', "tab",     NULL,   "Use leading tabs." },
    { '2', NULL,      NULL,   "Set tab size to 2 spaces." },
    { '4', NULL,      NULL,   "Set tab size to 4 spaces (default)." },
    { '8', NULL,      NULL,   "Set tab size to 8 spaces." },
    { 'x', NULL,      NULL,   NULL },

};
Opts optSet{optList, "    "};

/**
 * Display help message.
 *
 * @param  name - of application.
 */
int Config::help(const std::string & error)
{
    std::cout << "Usage: " << name << " [Options]\n";
    std::cout << '\n';
    std::cout << "  Analyzes the given file for consistent leading whitespace and line endings.\n";
    std::cout << "  Corrects leading whitespace and line endings as required.\n";
    std::cout << '\n';
    std::cout << "  Options:\n";
    std::cout << optSet;

    if (error.empty())
        return 1;

    std::cerr << "\nError: " << error << "\n";

    return -1;
}


/**
 * Process command line parameters with help from Opts.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int Config::parseCommandLine(int argc, char *argv[])
{
    setName(argv[0]);   // Store program name;
    if (argc < 2)
        return help("valid arguments required.");

    optSet.process(argc, argv);

    if (optSet.isErrors())
        return help("valid arguments required.");

    for (const auto & option : optSet)
    {
        switch (option.getOpt())
        {
        case 'h': return help("");
        case 'v': return version();

        case 'i': setInputFile(option.getArg()); break;
        case 'o': setOutputFile(option.getArg()); break;
        case 'r': setReplaceFile(option.getArg()); break;

        case 'd': setDos();     break;
        case 'u': setUnix();    break;

        case 's': setSpaces();  break;
        case 't': setTabs();    break;

        case '2': setTabSize(2);break;
        case '4': setTabSize(4);break;
        case '8': setTabSize(8);break;

        case 'x': enableDebug();break;

        default: return help("internal error.");
        }
    }

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
    os << "Application name: " << getName() << '\n';
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


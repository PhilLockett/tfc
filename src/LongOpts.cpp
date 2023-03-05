/**
 * @file    Portal.cpp
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
 * Implementation of LongOpts, a wrapper class providing a simplified interface
 * to getopt_long() as well as generating a help page options list.
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string_view>

#include "LongOpts.h"


/**
 * @brief Stream command line option descriptions.
 * 
 * @param os output stream.
 */
void LongOpts::display(std::ostream &os) const
{
    // Calculate longest preamble.
    size_t longest{};
    for (const LongOpt & i : list)
    {
        int length{1};
        if (i.isName())
            length += i.getNameLen() + 2;

        if (i.isArg())
            length += i.getArgLen() + 3;

        if (length > longest)
            longest = length;
    }

    const std::string padding(longest, ' ');

    // Output options.
    for (const LongOpt & i : list)
    {
        os << indent << "-" << i.getVal() << " ";

        std::string work{};
        if (i.isName())
            work += "--" + i.getNameString();

        if (i.isArg())
            work += " <" + i.getArgString() + ">";
            
        work += padding;
        const std::string_view view{work.c_str(), longest};

        os << view << i.getDesc() << "\n";
    }
}

/**
 * @brief Fill in the long_options vector from provided list.
 */
void LongOpts::fillOptions(void)
{
    long_options.reserve(list.size());
    struct option long_option;
    long_option.flag = 0;
    for (const LongOpt & i : list)
    {
        if (i.isName())
        {
            long_option.name = i.getName();
            long_option.has_arg = i.isArg() ? required_argument : no_argument;
            long_option.val = i.getVal();
            long_options.push_back(long_option);
        }
    }

    long_option.name = 0;
    long_option.has_arg = 0;
    long_option.val = 0;
    long_options.push_back(long_option);

}

/**
 * @brief Generate the optstring from provided list.
 * 
 * @return std::string acceptable option characters.
 */
std::string LongOpts::getOptstring(void) const
{
    std::string opts{};

    for (const LongOpt & i : list)
    {
        opts += i.getVal();
        if (i.isArg())
            opts += ":";
    }

    return opts;
}

LongOpts::LongOpts(const std::vector<LongOpt> & l, const std::string i, const std::string extras)
 : list{l}, indent{i}, optstring{getOptstring()+extras}
{
    reset();
    fillOptions();
}

int LongOpts::getOpt(int argc, char *argv[]) const
{
    int option_index{};

    return getopt_long(argc, argv, optstring.c_str(), long_options.data(), &option_index);
}


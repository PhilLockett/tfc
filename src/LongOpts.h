/**
 * @file    LongOpts.h
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
 * Interface to and implementation of the LongOpt and LongOpts.
 */

#if !defined(_LONGOPTS_C_H__20230304_1805__INCLUDED_)
#define _LONGOPTS_C_H__20230304_1805__INCLUDED_

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <getopt.h>


class LongOpt
{
private:
    const int    val;
    const char * name;
    const char * arg;
    const char * desc;

public:
    LongOpt(int v, const char * n, const char * a, const char * d) : val{v}, name{n}, arg{a}, desc{d}
    {}

    int getValInt(void) const { return val; }
    char getVal(void) const { return (char)val; }

    const char * getName(void) const { return name; }
    const char * getArg(void) const { return arg; }
    const char * getDesc(void) const { return desc; }

    std::string getNameString(void) const { return std::string(getName()); }
    std::string getArgString(void) const { return std::string(getArg()); }
    std::string getDescString(void) const { return std::string(getDesc()); }

    bool isName(void) const { return getName() != NULL; }
    bool isArg(void) const { return getArg() != NULL; }
    bool isDesc(void) const { return getDesc() != NULL; }

    int getNameLen(void) const { return strlen(getName()); }
    int getArgLen(void) const { return strlen(getArg()); }
    int getDescLen(void) const { return strlen(getDesc()); }

};


class LongOpts
{
private:
    std::vector<struct option> long_options;
    const std::vector<LongOpt> & list;
    const std::string indent;
    const std::string optstring;

    void fillOptions(void);
    std::string getOptstring(void) const;

    void display(std::ostream &os) const;

public:
    /**
     * @brief Construct a new Long Opts object
     * 
     * @param list Vector of LongOpt enries describing each option.
     * @param indent string displayed at the begining of each line.
     * @param extras string of additional single character options.
     */
    LongOpts(const std::vector<LongOpt> & list, const std::string indent, const std::string extras);

    /**
     * @brief Reset the argv index to the start of the command line parameters.
     */
    void reset(void) const { optind = 1; } // Set the index.

    /**
     * @brief Get the next option in argv.
     * 
     * @param  argc - command line argument count.
     * @param  argv - command line argument vector.
     * @return int the next option character.
     */
    int getOpt(int argc, char *argv[]) const;

    /**
     * @brief Get the argument associated with the last option returned by getOpt().
     * 
     * @return std::string the argument.
     */
    std::string getArg(void) const { return std::string(optarg); }

    friend std::ostream & operator<<(std::ostream &os, const LongOpts &A) { A.display(os); return os; }

};


#endif //!defined _LONGOPTS_C_H__20230304_1805__INCLUDED_


/**
 * @file    tfc.h
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
 * Common interfaces for the text file checker.
 */

#if !defined _TFC_H_INCLUDED_
#define _TFC_H_INCLUDED_

#include <iostream>
#include <string>

#if !defined ELEMENTS
#define ELEMENTS(A) (sizeof(A)/sizeof(A[0]))
#endif



/**
 * @section tfc configuration data.
 *
 * Implementation of the tfc comand line configuration Singleton.
 */

class Config
{
private:
//- Hide the default constructor and destructor.
    Config(void) : inputFile{}, outputFile{}, leading{Whitespace::unspecified}, trailing{EndOfLine::unspecified}
        {  }
    virtual ~Config(void) {}

    void display(std::ostream &os) const;

    enum class Whitespace { unspecified, space, tab };
    enum class EndOfLine { unspecified, dos, unix };

    std::string inputFile;
    std::string outputFile;
    Whitespace leading;
    EndOfLine trailing;

    void setInputFile(std::string name) { Config::get().inputFile = name; }
    void setOutputFile(std::string name) { Config::get().outputFile = name; }
    void setSpaces() { Config::get().leading = Whitespace::space; }
    void setTabs() { Config::get().leading = Whitespace::tab; }
    void setDos() { Config::get().trailing = EndOfLine::dos; }
    void setUnix() { Config::get().trailing = EndOfLine::unix; }

public:
//- Delete the copy constructor and assignement operator.
    Config(const Config &) = delete;
    void operator=(const Config &) = delete;

    friend int parseCommandLine(int argc, char *argv[], Config &config);
    friend std::ostream & operator<<(std::ostream &os, const Config &A) { A.display(os); return os; }

    static Config & get() { static Config instance; return instance; }

    static std::string getInputFile(void)     { return Config::get().inputFile; }
    static std::string getOutputFile(void)     { return Config::get().outputFile; }

    static bool isLeadingSet(void) { return Config::get().leading != Whitespace::unspecified; }
    static bool isSpace(void) { return Config::get().leading == Whitespace::space; }
    static bool isTab(void) { return Config::get().leading == Whitespace::tab; }

    static bool isTrailingSet(void) { return Config::get().trailing != EndOfLine::unspecified; }
    static bool isDos(void) { return Config::get().trailing == EndOfLine::dos; }
    static bool isUnix(void) { return Config::get().trailing == EndOfLine::unix; }

    static bool isChangeRequested(void) { return isLeadingSet() || isTrailingSet(); }

    static bool isValid(void) { return !Config::get().inputFile.empty();}


};



/**
 * @section Common functions.
 *
 */
extern int init(int argc, char *argv[]);

namespace transform
{
    extern int process(void);
}

namespace summary
{
    extern int process(void);
}

#endif //!defined _TFC_H_INCLUDED_


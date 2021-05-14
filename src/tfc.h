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
#include <filesystem>

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
    Config(void) : 
        inputFile{}, outputFile{}, replace{},
        leading{Whitespace::unspecified}, trailing{EndOfLine::unspecified},
        tabSize{4}, debug{}
        {  }
    virtual ~Config(void) {}

    void display(std::ostream &os) const;

    enum class Whitespace { unspecified, space, tab };
    enum class EndOfLine { unspecified, dos, unix };

    std::filesystem::path inputFile;
    std::filesystem::path outputFile;
    bool replace;
    Whitespace leading;
    EndOfLine trailing;
    size_t tabSize;
    bool debug;

    void setInputFile(std::string name) { Config::instance().inputFile = name; }
    void setOutputFile(std::string name) { Config::instance().outputFile = name; }
    void setReplaceFile(std::string name) { Config::instance().inputFile = name; replace = true; }
    void setSpaces() { Config::instance().leading = Whitespace::space; }
    void setTabs() { Config::instance().leading = Whitespace::tab; }
    void setDos() { Config::instance().trailing = EndOfLine::dos; }
    void setUnix() { Config::instance().trailing = EndOfLine::unix; }
    void setTabSize(size_t size) { tabSize = size; }
    void enableDebug(void) {debug = true; }

public:
//- Delete the copy constructor and assignement operator.
    Config(const Config &) = delete;
    void operator=(const Config &) = delete;

    friend int parseCommandLine(int argc, char *argv[], Config &config);
    friend std::ostream & operator<<(std::ostream &os, const Config &A) { A.display(os); return os; }

    static Config & instance() { static Config neo; return neo; }

    static std::filesystem::path & getInputFile(void)     { return Config::instance().inputFile; }
    static std::filesystem::path & getOutputFile(void)    { return Config::instance().outputFile; }
    static bool isReplacing(void) { return Config::instance().replace; }

    static bool isLeadingSet(void) { return Config::instance().leading != Whitespace::unspecified; }
    static bool isSpace(void) { return Config::instance().leading == Whitespace::space; }
    static bool isTab(void) { return Config::instance().leading == Whitespace::tab; }

    static bool isTrailingSet(void) { return Config::instance().trailing != EndOfLine::unspecified; }
    static bool isDos(void) { return Config::instance().trailing == EndOfLine::dos; }
    static bool isUnix(void) { return Config::instance().trailing == EndOfLine::unix; }

    static size_t getTabSize(void) { return Config::instance().tabSize; }

    static bool isChangeRequested(void) { return isLeadingSet() || isTrailingSet(); }
    static bool isSummary(void) { return !isChangeRequested(); }

    static bool isValid(bool showErrors = false);
    static bool isDebug(void) { return Config::instance().debug; }

};



/**
 * @section Common functions.
 *
 */
extern int init(int argc, char *argv[]);

extern int processTransform(void);
extern int processSummary(void);

#endif //!defined _TFC_H_INCLUDED_


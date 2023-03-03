/**
 * @file    configuration.h
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

#if !defined _CONFIGURATION_H_INCLUDED_
#define _CONFIGURATION_H_INCLUDED_

#include <iostream>
#include <string>
#include <filesystem>



/**
 * @section tfc configuration Singleton.
 */

class Config
{
private:
//- Hide the default constructor and destructor.
    Config(void) : 
        name{}, inputFile{}, outputFile{}, replace{},
        leading{Whitespace::unspecified}, trailing{EndOfLine::unspecified},
        tabSize{4}, debug{}
        {  }
    virtual ~Config(void) {}

    void display(std::ostream &os) const;

    enum class Whitespace { unspecified, space, tab };
    enum class EndOfLine { unspecified, dos, unix };

    std::string name;
    std::filesystem::path inputFile;
    std::filesystem::path outputFile;
    bool replace;
    Whitespace leading;
    EndOfLine trailing;
    size_t tabSize;
    bool debug;

    void setName(std::string value) { name = value; }
    void setInputFile(std::string name) { Config::instance().inputFile = name; }
    void setOutputFile(std::string name) { Config::instance().outputFile = name; }
    void setReplaceFile(std::string name) { Config::instance().inputFile = name; replace = true; }
    void setSpaces() { Config::instance().leading = Whitespace::space; }
    void setTabs() { Config::instance().leading = Whitespace::tab; }
    void setDos() { Config::instance().trailing = EndOfLine::dos; }
    void setUnix() { Config::instance().trailing = EndOfLine::unix; }
    void setTabSize(size_t size) { tabSize = size; }
    void enableDebug(void) {debug = true; }

    int version(void);
    int help(void);
    int parseCommandLine(int argc, char *argv[]);

public:
//- Delete the copy constructor and assignement operator.
    Config(const Config &) = delete;
    void operator=(const Config &) = delete;

    int init(int argc, char *argv[]);
    friend std::ostream & operator<<(std::ostream &os, const Config &A) { A.display(os); return os; }

    static Config & instance() { static Config neo; return neo; }

    static std::string & getName(void) { return instance().name; }
    static std::filesystem::path & getInputFile(void)     { return Config::instance().inputFile; }
    static std::filesystem::path & getOutputFile(void)    { return Config::instance().outputFile; }

    static bool isName(void) { return !instance().name.empty(); }
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


#endif //!defined _CONFIGURATION_H_INCLUDED_


# tfc
`tfc` is a command-line utility Text File Checker and corrector.

**THIS UTILITY CAN BE USED TO OVERWRITE EXISTING FIES.**

**USE AT OWN RISK.**

## Overview
This utility checks text files for consistency in the following areas:
  * leading whitespace
  * line endings
  * identifies presence of UTF-8 or ISO/IEC 8859-1 characters

It can either report on the state of a file or correct inconsistencies
depending on the options provided.
It does not change character encodings (UTF-8 or ISO/IEC 8859-1) but the
summary will identify the presence of either.

To use `tfc` you will need a C++ compiler and 'make' utility installed. 

## Usage
With `tfc` installed the following command will display the help page:

    $ tfc --help
    Usage: tfc [Options]
    
      Analyzes the given file for consistent leading whitespace and line endings.
      Corrects leading whitespace and line endings as required.
    
      Options:
        -h --help           This help page and nothing else.
        -v --version        Display version.

        -i --input <file>   Input file name.
        -o --output <file>  Output file name (default: console).
        -r --replace <file> Replace file with transformed version.
        -d --dos            DOS style End-Of-line.
        -u --unix           Unix style End-Of-line.
        -s --space          Use leading spaces.
        -t --tab            Use leading tabs.
        -2                  Set tab size to 2 spaces.
        -4                  Set tab size to 4 spaces (default).
        -8                  Set tab size to 8 spaces.

### Changing the leading whitespace
The leading whitespace of every line can be changed to either all spaces or
tabs with the minimum number of spaces to make up the difference. The options
`-2`, `-4` or `-8` set the tab size in characters. The column offset is
calculated from the leading whitespace. If a tab is encountered the offset is
incremented to the next tab stop based on tab size. When a non whitespace
character is found the column offset is used to create leading whitespace
padding from tabs and/or spaces. The rest of the line is left unchanged.

### Changing the newline characters.
The trailing new line can be changed to either a dos style carriage return line
feed or a unix style line feed only even if the file contains a mix of these.

### Summary generation
If neither a whitespace or newline change is requested a summary is generated
indicating the total number of lines, the number of lines that start with
spaces or tabs or both and the number of lines the end with dos or unix
newlines.

### File specification
An input file must be specified. Output is sent to the console unless an output
file is specified, this includes summary generation. The input file and the 
output file cannot be the same file. If the input file is to be overwritten
with the output, use the replace option to specify the file instead of the
input and output options.

## Cloning and Installing
To clone, install and run this code, execute the following unix/linux commands:

    $ git clone https://github.com/PhilLockett/tfc.git
    $ cd tfc/
    $ chmod +x fix.sh
    $ ./fix.sh
    $ ./configure
    $ make
    $ sudo make install

Once installed, `tfc` can be run from any directory.

## Creating a tar file
Sometimes it is more convenient to use a tar file to share software than 
connect to GitHub. The following command will create a tar file called 
'tfc-1.0.tar.gz' from a cloned copy of the code:

    $ cd tfc/
    $ make dist

## Installing from tar file
The install process described in the INSTALL document is generic. For 
completeness the install process is also described here.

The following command will unpack the tar file.

    $ tar zxf tfc-1.0.tar.gz

After unpacking, issue the following shell commands:

    $ cd tfc-1.0/
    $ ./configure
    $ make
    $ sudo make install

## Uninstalling and cleaning up
To uninstall `tfc`, run the following command from the directory used to run
'./configure', 'make' and 'make install':

    $ sudo make uninstall

Once uninstalled, delete the 'tfc' directory and all it's contents (it can easily
be cloned again).

## Examples
### Checking the state of a file
To check a file:

    $ tfc -i tfc.cpp
    tfc.cpp
      Total Lines:  75
    Line beginning:
      Space only:   55
      Neither:      20
    Line ending:
      Unix:         75

### Generate a new file
To generate a new file with tabs (with spaces as needed) at the beginning of 
lines and Dos style line endings.

    $ tfc -i tfc.cpp -t -d -o tfcdos.cpp

This creates the new file 'tfcdos.cpp':

    $ tfc -i tfcdos.cpp
    tfcdos.cpp
      Total Lines:  75
    Line beginning:
      Space only:   32
      Tab only:     23
      Neither:      20
    Line ending:
      Dos:          75

The default tab size is 4 spaces. 
Every occurrence of 4 spaces is replaced with a tab. 
Any line that starts with less than 4 spaces will not have the spaces changed.

### Attempting to overwrite an existing file
If the output option specifies an existing file `tfc` will generate an error 
message and abort:

    $ tfc -i tfc.cpp -8 -t -d -o tfcdos.cpp

    Warning output file "tfcdos.cpp" will be overwritten.

### Changing the tab size
The number of spaces to be replaced by tabs can be specified as 2, 4 or 8:

    $ rm tfcdos.cpp
    $ tfc -i tfc.cpp -8 -t -d -o tfcdos.cpp
    $ tfc -i tfcdos.cpp
    tfcdos.cpp
      Total Lines:  75
    Line beginning:
      Space only:   50
      Tab only:     5
      Neither:      20
    Line ending:
      Dos:          75

Here we delete 'tfcdos.cpp' before we generate it again with tabs the size of 8 
spaces.

### Overwriting existing files
Sometimes it is more convenient to overwrite a file with the changes:

    $ tfc -r tfcdos.cpp -t -d
    $ tfc -i tfcdos.cpp
    tfcdos.cpp
      Total Lines:  75
    Line beginning:
      Space only:   32
      Tab only:     23
      Neither:      20
    Line ending:
      Dos:          75

**THIS IS NOT RECOMMENDED UNLESS YOU HAVE A BACK UP AND CAN TRACK THE CHANGES**

## Points of interest
This code has the following points of interest:

  * Uses Opts to help handle command line parameters.
  * The command line parameters are stored in the Config class.
  * The Config class is implemented as a singleton.
  * The Config setters are private so only methods and friends can use them.

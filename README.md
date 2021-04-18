# tfc
'tfc' is a command-line utility Text File Checker.

## Overview
The utility checks text files for consistent leading whitespace and line 
endings.

To use 'tfc' you will need a C++ compiler and 'make' utility installed. 

## Cloning and Installing
To clone, install and run this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/tfc.git
    cd tfc/
	chmod +x configure
    ./configure
    make
    sudo make install

## Creating a tar file
Sometimes it is more convenient to use a tar file to share software than 
connect to GitHub. The following command will create a tar file called 
'tfc-1.0.tar.gz' from a cloned copy of the code:

    cd tfc/
    make dist

## Installing from tar file
The install process described in the INSTALL document is a generic 
description of the install process. The install process is also described 
in the README.md document. For completeness the install process is also 
described below. All three descriptions are essentially the same and clearly, 
it only needs to be installed once.

The following command will unpack the tar file.

    tar zxf tfc-1.0.tar.gz

After unpacking, issue the following shell commands:

    cd tfc-1.0/
    ./configure
    make
    sudo make install

## Setting up 'tfc' environment and Running
With 'tfc' installed the following command will display the help page:

    tfc --help

## Points of interest
This code has the following points of interest:

  * Uses getopt_long() to help handle command line parameters.
  * The command line parameters are stored in the Config class.
  * The Config class is implemented as a singleton.
  * The Config setters are private so only methods and friends can use them.

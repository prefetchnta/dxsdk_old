SDK Samples
-----------

All SDK samples are designed to be built in the directory they are in.  
Just ensure that your Win32 development environment is set up, and you 
can go to any sample directory and do a make.

There are 2 main make files in most sample directories:
MAKEFILE        - for use with Microsoft VC++ 2.0 (NMAKE)
MAKEFILE.WAT    - for use with Watcom C/C++ 10.0 (WMAKE)

There are 2 ways to build each sample, debug or retail.   To build a
sample as retail with VC++, just type:

nmake retail

To build a sample as debug with Watcom C/C++, just type:

wmake /f makefile.wat debug

From the SAMPLES directory, you may also run the following batch files:

MAKEALL.BAT     - makes all examples with Microsoft VC++
MAKEALLW.BAT    - makes all examples with WATCOM C/C++

PLEASE NOTE: for this beta release, there is no makefile to build the 
following samples with the WATCOM compiler.

- Immortal Klowns 
- Bee
- DPTest

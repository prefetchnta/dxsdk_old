@echo *
@echo ***Batch File to build examples with WATCOM C/C++ 
@echo *
cd ddex1
wmake /f makefile.wat
cd ..\ddex2
wmake /f makefile.wat
cd ..\ddex3
wmake /f makefile.wat
cd ..\ddex4
wmake /f makefile.wat
cd ..\ddex5
wmake /f makefile.wat
cd ..\foxbear
wmake /f makefile.wat
cd ..\font
wmake /f makefile.wat
cd ..\fastfile
wmake /f makefile.wat
cd ..\palette
wmake /f makefile.wat
cd ..\dsshow
wmake /f makefile.wat
cd ..\mid2strm
wmake /f makefile.wat

@echo off

REM Setup build directory
if not exist build ( mkdir build )
cd build

cl ..\*.cpp /w /nologo /Od /Zi /DAPAD_INTERNAL /Fetest /std:c++17 /link user32.lib opengl32.lib dbghelp.lib gdi32.lib

exit /b REM Exit batch script

:Help
echo:
echo Function: Compiles and links all translations units for testing.
echo:
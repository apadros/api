@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )
if %3.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

REM Release version
REM Compile all translation units into object files first
cl /w /nologo /O2 /c /std:c++17 ..\apad_*.cpp

REM Link all into single library
cl /nologo /w /LD /Fe: apad_api_v%1.%2.%3.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib >> temp.txt

REM Debug version including stack back tracing
cl /w /nologo /Od /Zi /DAPAD_ASSERTIONS_BACKTRACE /c /std:c++17 ..\apad_*.cpp
cl /nologo /w /Od /Zi /LD /Fe: apad_api_v%1.%2.%3_debug.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib >> temp.txt

del temp.txt
del *.obj
del *.exp
del *.ilk
del vc*.pdb

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
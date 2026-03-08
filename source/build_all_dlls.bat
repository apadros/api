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

REM Compile all translation units into object files first
cl /w /nologo /O2 /c /std:c++17 ..\*.cpp

del test.obj

REM Link all into single library
cl /nologo /w /LD /Fe: apad_api_v%1.%2.%3.dll *.obj /link user32.lib gdi32.lib opengl32.lib >> temp.txt

del temp.txt
del *.obj
del *.exp

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_all_dlls.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
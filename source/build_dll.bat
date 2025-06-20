@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

cl /O2 /LD ..\%1 /Fe%2 ..\..\lib64\apad_debug_v0_2_0.lib ..\..\lib64\apad_win32_v0_2_0.lib

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat file.cpp exe_name
echo:
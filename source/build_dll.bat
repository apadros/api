@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

cl /O2 /LD ..\%1 ..\..\lib64\apad_error.lib ..\..\lib64\apad_memory.lib ..\..\lib64\apad_string.lib
if exist *.lib (
	move *.lib ..\..\bin
	move *.dll ..\..\bin
)

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Function: Compiles to lib and dll and moves them to the bin folder
echo:
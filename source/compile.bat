@echo off

if %1.==. ( goto Help )

REM Setup build directory
if not exist build ( mkdir build )
pushd build

cl ..\%1 /c /w /nologo /std:c++17

popd
exit /b REM Exit batch script

:Help
echo:
echo Function: Only checks compilation of target translation unit.
echo:
echo Usage: compile file.cpp
echo:
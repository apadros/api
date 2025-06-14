@echo off

if %1.==. ( goto Help )

REM Setup build directory
if not exist build ( mkdir build )
pushd build

cl ..\%1 /c

popd
exit /b REM Exit batch script

:Help
echo:
echo Function: Only checks compilation for target file.
echo:
echo Usage: compile file_without_extension
echo:
@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )
if %3.==. ( goto Help )

if exist release (rmdir release /s /q)
mkdir release
mkdir release\bin
mkdir release\source

pushd source\
call build_dll_release.bat %1 %2 %3
popd

echo:
echo MOVING...
echo:

move source\build\* release\bin\
copy source\*.h release\source\
del  release\source\apad_*_internal.h

exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: prepare_release.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
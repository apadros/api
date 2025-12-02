@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )
if %3.==. ( goto Help )

if not exist release ( mkdir release )
del release\* /q

pushd source\
call build_all_dlls.bat %1 %2 %3
popd

echo:
echo MOVING...
echo:

move source\build\* release\
copy source\*.h release\

exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: prepare_release.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
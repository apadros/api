@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )
if %3.==. ( goto Help )

if not exist release ( mkdir release )
del release\*.h /q
del release\*.lib /q
del release\*.dll /q
del release\*.pdb /q

pushd source\
call build_dll_release.bat %1 %2 %3
popd

echo:
echo MOVING...
echo:

move source\build\* release\
copy source\*.h release\
del  release\apad_error_internal.h

exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: prepare_release.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
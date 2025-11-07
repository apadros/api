@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

echo:
echo #################### ERROR ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_error.cpp /Feapad_error_%1

echo:
echo #################### TEMP #####################
echo:

REM Make temp libs
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_%1 >> temp.txt
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_%1 >> temp.txt

echo:
echo #################### WIN32 ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_win32.cpp /Feapad_win32_%1 apad_error_%1.lib apad_memory_%1.lib apad_string_%1.lib

echo:
echo #################### MEMORY ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_%1 apad_error_%1.lib apad_win32_%1.lib

echo #################### FILE ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_file.cpp /Feapad_file_%1 apad_memory_%1.lib apad_win32_%1.lib

echo:
echo #################### STRING ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_%1 apad_error_%1.lib apad_memory_%1.lib

echo:
echo #################### LOGGING ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_logging.cpp /Feapad_logging_%1 apad_error_%1.lib apad_memory_%1.lib apad_string_%1.lib

echo:
echo #################### TIME ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_time.cpp /Feapad_time_%1 apad_error_%1.lib apad_string_%1.lib


del temp.txt
del *.obj
del *.exp

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat version(vx.y.z)
echo:
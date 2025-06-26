@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

echo:
echo ###############################################
echo #################### ERROR ####################
echo ###############################################
echo:

cl /O2 /LD /w ..\apad_error.cpp /Feapad_error_%1

echo:
echo ###############################################
echo #################### TEMP #####################
echo ###############################################
echo:

REM Make temp libs
cl /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_%1 >> temp.txt
cl /O2 /LD /w ..\apad_string.cpp /Feapad_string_%1 >> temp.txt


echo:
echo ###############################################
echo #################### WIN32 ####################
echo ###############################################
echo:

cl /O2 /LD /w ..\apad_win32.cpp /Feapad_win32_%1 apad_error_%1.lib apad_memory_%1.lib apad_string_%1.lib

echo:
echo ################################################
echo #################### MEMORY ####################
echo ################################################
echo:

cl /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_%1 apad_error_%1.lib apad_win32_%1.lib

echo:
echo ################################################
echo #################### STRING ####################
echo ################################################
echo:

cl /O2 /LD /w ..\apad_string.cpp /Feapad_string_%1 apad_error_%1.lib apad_memory_%1.lib

echo:
echo ################################################
echo #################### LOGGING ####################
echo ################################################
echo:

cl /O2 /LD /w ..\apad_logging.cpp /Feapad_logging_%1 apad_error_%1.lib apad_memory_%1.lib apad_string_%1.lib

del temp.txt
del *.obj
del *.exp

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat version(vx_y_z)
echo:
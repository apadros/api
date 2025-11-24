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

echo:
echo #################### ERROR ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_error.cpp /Feapad_error_v%1.%2.%3.dll

echo:
echo #################### TEMP #####################
echo:

REM Make temp libs
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_v%1.%2.%3.dll >> temp.txt
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_v%1.%2.%3.dll >> temp.txt

echo:
echo #################### WIN32 ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_win32.cpp /Feapad_win32_v%1.%2.%3.dll apad_error_v%1.%2.%3.lib apad_memory_v%1.%2.%3.lib apad_string_v%1.%2.%3.lib

echo:
echo #################### MEMORY ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_v%1.%2.%3.dll apad_error_v%1.%2.%3.lib apad_win32_v%1.%2.%3.lib

echo:
echo #################### FILE ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_file.cpp /Feapad_file_v%1.%2.%3.dll apad_memory_v%1.%2.%3.lib apad_win32_v%1.%2.%3.lib

echo:
echo #################### STRING ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_v%1.%2.%3.dll apad_error_v%1.%2.%3.lib apad_memory_v%1.%2.%3.lib

echo:
echo #################### LOG ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_log.cpp /Feapad_log_v%1.%2.%3.dll apad_error_v%1.%2.%3.lib apad_memory_v%1.%2.%3.lib apad_string_v%1.%2.%3.lib

echo:
echo #################### TIME ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_time.cpp /Feapad_time_v%1.%2.%3.dll apad_error_v%1.%2.%3.lib apad_string_v%1.%2.%3.lib


del temp.txt
del *.obj
del *.exp

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
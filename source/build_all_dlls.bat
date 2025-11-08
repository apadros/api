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

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_error.cpp /Feapad_error_v_%1_%2_%3

echo:
echo #################### TEMP #####################
echo:

REM Make temp libs
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_v_%1_%2_%3 >> temp.txt
cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_v_%1_%2_%3 >> temp.txt

echo:
echo #################### WIN32 ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_win32.cpp /Feapad_win32_v_%1_%2_%3 apad_error_v_%1_%2_%3.lib apad_memory_v_%1_%2_%3.lib apad_string_v_%1_%2_%3.lib

echo:
echo #################### MEMORY ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_memory.cpp /Feapad_memory_v_%1_%2_%3 apad_error_v_%1_%2_%3.lib apad_win32_v_%1_%2_%3.lib

echo:
echo #################### FILE ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_file.cpp /Feapad_file_v_%1_%2_%3 apad_memory_v_%1_%2_%3.lib apad_win32_v_%1_%2_%3.lib

echo:
echo #################### STRING ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_string.cpp /Feapad_string_v_%1_%2_%3 apad_error_v_%1_%2_%3.lib apad_memory_v_%1_%2_%3.lib

echo:
echo #################### LOGGING ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_logging.cpp /Feapad_logging_v_%1_%2_%3 apad_error_v_%1_%2_%3.lib apad_memory_v_%1_%2_%3.lib apad_string_v_%1_%2_%3.lib

echo:
echo #################### TIME ####################
echo:

cl /nologo /DAPAD_DLL /O2 /LD /w ..\apad_time.cpp /Feapad_time_v_%1_%2_%3 apad_error_v_%1_%2_%3.lib apad_string_v_%1_%2_%3.lib


del temp.txt
del *.obj
del *.exp

echo:
echo RENAMING...
echo:

REM For some reason, supplying the compiler with vx.y.z instead of v_x_y_z creates errors
ren *v_%1_%2_%3.lib *v%1.%2.%3.lib
ren *v_%1_%2_%3.dll *v%1.%2.%3.dll

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Usage: build_dll.bat [version_major_number] [version_minor_number] [version_patch_number]
echo:
@echo off

REM Check whether all arguments have been supplied
if %1.==. ( goto Help )
if %2.==. ( goto Help )

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

REM Compile to exe
if %2==exe (
	echo:
	cl /Od /Zi /I.. ..\%1.cpp /Fetest
	REM Open debugger only if an exe was created
	if exist test.exe (
		devenv test.exe
	)
)

REM Compile to dll
if %2==dll (
  cl /O2 /I.. ..\%1.cpp /Fe%1 /LD
	if exist %1.lib (
		move *.lib ..\..\bin
		move *.dll ..\..\bin
	)
)

popd
exit /b REM Exit batch script

REM Help message
:Help
echo:
echo Function: Compiles to either .exe for testing or .lib and .dll.
echo           In the latter case it will move the files to the bin folder.
echo:
echo Usage: build file_without_extension exe/dll
echo:
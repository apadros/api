@echo off

if %1.==. ( 
	echo:
	echo Usage: %0 [build.bat without the path]
	exit /b 
)

if exist release (rmdir release /s /q)
mkdir release
mkdir release\bin
mkdir release\source

pushd source\
if exist build (del build /q)
call %1
popd

echo:
echo MOVING...
echo:

move source\build\*.dll release\bin\
move source\build\*.lib release\bin\
move source\build\*.pdb release\bin\
if exist release\bin\vc140.pdb (del release\bin\vc140.pdb)
copy source\*.h release\source\
del  release\source\apad_*_internal.h

exit /b REM Exit batch script
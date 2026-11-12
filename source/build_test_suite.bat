@echo off

REM Assumed that the batch file will set the correct directory
call build_dll_testing.bat

del _test.*

REM kernel32.lib is needed for Sleep()
cl ..\testing.cpp /w /nologo /Od /Zi /std:c++17 /DAPAD_TESTING /Fe: _test.exe /link kernel32.lib winmm.lib *dll*.lib >> temp.txt

del *.obj

REM Build run_test.bat, set command needs to be run from here as it seems it doesn't work within the program
echo @echo off >> run_test.bat
echo set _current_date=%date% >> run_test.bat
echo set _current_time=%time% >> run_test.bat
echo _test.exe >> run_test.bat REM _text.exe will change and restore time and date internally
echo set _current_date= >> run_test.bat
echo set _current_time= >> run_test.bat


REM Build recompile.bat
echo @echo off > recompile.bat
echo cd .. >> recompile.bat
echo call compile.bat testing.cpp >> recompile.bat
echo cd build >> recompile.bat

REM Build rebuild.bat
echo @echo off > rebuild.bat
echo cd .. >> rebuild.bat
echo call %0 >> rebuild.bat
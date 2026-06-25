@echo off

REM Setup build directory
if not exist build ( mkdir build )
cd build
del dll_* /q

REM Standard debug version
cl /w /nologo /Od /Zi /DAPAD_ASSERTIONS_BACKTRACE /c /std:c++17 ..\apad_*.cpp
cl /nologo /w /Od /Zi /LD /Fe: dll_debug.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib comdlg32.lib shlwapi.lib >> temp.txt

del *.obj
del *.exp
del *.ilk
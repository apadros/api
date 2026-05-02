@echo off

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

REM Standard debug version
cl /w /nologo /Od /Zi /DAPAD_ASSERTIONS_BACKTRACE /c /std:c++17 ..\apad_*.cpp
cl /nologo /w /Od /Zi /LD /Fe: dll_debug.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib >> temp.txt

REM Version for the debugger
cl /w /nologo /Od /Zi /DAPAD_DEBUGGER_ASSERTIONS /c /std:c++17 ..\apad_*.cpp
cl /nologo /w /Od /Zi /LD /Fe: dll_debug_assertions.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib >> temp.txt


del temp.txt
del *.obj
del *.exp
del *.ilk

popd
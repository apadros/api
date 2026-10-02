@echo off

REM Setup build directory
if not exist build ( mkdir build )
cd build
del * /q

cl /w /nologo /Od /Zi /DAPAD_DEBUGGER_ASSERTIONS /std:c++17 ..\apad_*.cpp /LD /Fe: dll_debug.dll /link user32.lib gdi32.lib opengl32.lib dbghelp.lib comdlg32.lib shlwapi.lib >> temp.txt

del *.obj
del *.exp
del *.ilk
@echo off

REM Setup build directory
if not exist build ( mkdir build )
cd build
del * /q

cl /w /nologo /Od /Zi /DAPAD_TESTING /DAPAD_ASSERTIONS_BACKTRACE /std:c++17 ..\apad_*.cpp /LD /Fe: dll_debug_testing.dll /link user32.lib gdi32.lib opengl32.lib dbghelp.lib comdlg32.lib shlwapi.lib >> temp.txt

if not exist *.dll (
	echo:
	echo ERROR: The library was not built
	echo:
	exit /b
)

del *.obj
del *.exp
del *.ilk
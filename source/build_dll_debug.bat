@echo off

cls

REM Setup build directory
if not exist build ( mkdir build )
del build\* /q
pushd build

REM Compile all translation units into object files first
cl /w /nologo /Od /Zi /DAPAD_ASSERTIONS_BACKTRACE /c /std:c++17 ..\*.cpp

del test.obj

REM Link all into single library
cl /nologo /w /Od /Zi /LD /Fe: dll_debug.dll *.obj /link user32.lib gdi32.lib opengl32.lib dbghelp.lib >> temp.txt

del temp.txt
del *.obj
del *.exp

popd
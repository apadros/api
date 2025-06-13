#ifndef APAD_WIN32_H
#define APAD_WIN32_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"

// The first argument corresponds to the program name
#define ConsoleAppEntryPoint(_argumentsID, _argumentCountID) int main(int _argumentCountID, char** _argumentsID)

// ******************** Memory ********************  //


// Will automatically clear allocated memory
imported_function void* AllocateWin32Memory(ui32 size);
imported_function void  FreeWin32Memory(void* mem);

#endif
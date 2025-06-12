#ifndef APAD_WIN32_H
#define APAD_WIN32_H

#include "apad_os_types.h"

// The first argument corresponds to the program name
#define ConsoleAppEntryPoint(_argumentsID, _argumentCountID) int main(int _argumentCountID, char** _argumentsID)

// ******************** Memory ********************  //

void* AllocateWin32Memory(ui32 size);
void  FreeWin32Memory(void* mem);

#endif
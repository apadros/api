#ifndef APAD_WIN32_H
#define APAD_WIN32_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// The first argument corresponds to the program name
#define ConsoleAppEntryPoint(_argumentsID, _argumentCountID) int main(int _argumentCountID, char** _argumentsID)

// ******************** Memory ********************  //

												// Will automatically clear allocated memory
imported_function void* Win32AllocateMemory(ui32 size);
imported_function void  Win32FreeMemory(void* mem);

// ******************** Files ********************  //

imported_function bool 				 Win32FileExists(const char* path);
															 // Calls Win32FileExists() first, returns if false
imported_function memory_block Win32LoadFile(const char* path);
															 // Will create a new file if it doesn't exist. 
															 // If it does it'll get replaced.
imported_function void 				 Win32SaveFile(void* data, ui32 dataSize, const char* path);

#endif
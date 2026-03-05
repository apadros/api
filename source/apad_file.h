#ifndef APAD_FILE_H
#define APAD_FILE_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

typedef memory_block file;

dll_import bool (*FileExists)(const char* path);
dll_import file (*LoadFile)(const char* path); // Calls FileExists() first, returns if false
dll_import void (*SaveFile)(void* data, ui32 dataSize, const char* path); // Will create a new file if it doesn't exist. 
																																								 // If it does it'll get replaced.
dll_import void (*FreeFile)(file& f);
dll_import bool IsValid(file f); // Defined in memory.cpp

#endif
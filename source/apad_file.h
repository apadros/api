#ifndef APAD_FILE_H
#define APAD_FILE_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

typedef memory_block file;

imported_function bool (*FileExists)(const char* path);
imported_function file (*LoadFile)(const char* path); // Calls FileExists() first, returns if false
imported_function void (*SaveFile)(void* data, ui32 dataSize, const char* path); // Will create a new file if it doesn't exist. 
																																								 // If it does it'll get replaced.
imported_function void (*FreeFile)(file& f);
imported_function bool IsValid(file f); // Defined in memory.cpp

#endif
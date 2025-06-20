#ifndef APAD_FILE_H
#define APAD_FILE_H

#include "apad_base_types.h"
#include "apad_memory.h"
imported_function bool 				 (*FileExists)(const char* path);
imported_function memory_block (*LoadFile)(const char* path);
imported_function void 				 (*SaveFile)(void* data, ui32 dataSize, const char* path);

#endif
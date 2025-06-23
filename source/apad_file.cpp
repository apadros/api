#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

exported_function bool 				 (*FileExists)(const char* path) = Win32FileExists;
exported_function memory_block (*LoadFile)(const char* path) = Win32LoadFile;
exported_function void 				 (*SaveFile)(void* data, ui32 dataSize, const char* path) = Win32SaveFile;
exported_function void 				 (*FreeFile)(memory_block& file) = FreeMemory;

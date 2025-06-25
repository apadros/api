#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_file.h"
#include "apad_win32.h"

exported_function bool (*FileExists)(const char* path) = Win32FileExists;
exported_function file (*LoadFile)(const char* path) = Win32LoadFile;
exported_function void (*SaveFile)(void* data, ui32 dataSize, const char* path) = Win32SaveFile;
exported_function void (*FreeFile)(file& f) = FreeMemory;

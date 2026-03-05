#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_file.h"
#include "apad_memory.h"
#include "apad_win32.h"

dll_export program_external bool (*FileExists)(const char* path) = Win32FileExists;
dll_export program_external file (*LoadFile)(const char* path) = Win32LoadFile;
dll_export program_external void (*SaveFile)(void* data, ui32 dataSize, const char* path) = Win32SaveFile;
dll_export program_external void (*FreeFile)(file& f) = FreeMemory;
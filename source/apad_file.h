#ifndef APAD_FILE_H
#define APAD_FILE_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

const ui8 MaxFileLineElements = 32;
struct file_line {
	const char* data[MaxFileLineElements]; // @TODO - Make variable length
	ui8         count;
};

typedef memory_block file;

// A macro can be added here in case of porting to another OS

// For some reason can't declase these function pointers as dll_import
program_unique bool 			(*FileExists)(const char* path) = Win32FileExists;
program_unique file 			(*LoadFile)(const char* path) = Win32LoadFile; // FileExists() must be called first
program_unique void 			(*SaveFile)(void* data, ui32 dataSize, const char* path) = Win32SaveFile; // Will create a new file if it doesn't exist; if it does it'll get replaced.
program_unique void 			(*FreeFile)(file& f) = FreeMemory;
dll_import 		 bool   			IsValid(file f); // Defined in apad_memory.cpp
dll_import 		 const char* GetFileNameAndExtension(const char* path);

dll_import file_line ReadLine(file& f, ui32& readIndex);
dll_import bool      LineIsValid(file_line& f);

#endif
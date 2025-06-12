#include "apad_base_types.h"
#include "apad_intrinsics.h"

// Adjust the linkage before adding apad_memory.h
#ifdef imported_function
#undef imported_function
#endif
#define imported_function exported_function
#include "apad_memory.h"

// Copy these to apad_memory_types.h if changed
#define KB(value) (value * 1024)
#define MB(value) (KB(value) * 1024)
#define ClearStruct(_s) ClearMemory(&(_s), sizeof(_s))

// #include "apad_debug_error.h"
exported_function void ClearMemory(void* memory, memory_size size) {
  // Assert(memory != Null);
  // Assert(size > 0);
	if(memory == Null || size == Null)
		return;

  for (memory_size it = 0; it < size; it++)
		((ui8*)memory)[it] = 0;
}

exported_function void CopyMemory(void* source, memory_size size, void* destination) {
  // Assert(source != Null);
  // Assert(size > 0);
  // Assert(destination != Null);
	if(source == Null || size == Null || destination == Null)
		return;
  
  ui8* ps = (ui8*)source;
  ui8* pd = (ui8*)destination;
  
  for (memory_size it = 0; it < size; it++)
    pd[it] = ps[it];
}

#include <stdlib.h>
exported_function memory_block AllocateMemory(memory_size size, b8 clear) {
	// For the time being use malloc, later add custom Win32 implementation
	// @TODO - Call into custom Win32 API
	
	auto failRet = NullMemoryBlock;
	
	if(size == 0)
		return failRet;
	
	void* memory = malloc(size);
	if(memory == Null)
		return failRet;
	
	if(clear == true)
		ClearMemory(memory, size);
	
	memory_block ret;
	ClearStruct(ret);
	ret.memory = memory;
	ret.size = size;
	
	return ret;
}

#include <stdlib.h>
exported_function void FreeMemory(memory_block block) {
	// @TODO - When switching to the Win32 API for memory allocation, update this too
  if(block.memory == Null)
		return;
	free(block.memory);
}

exported_function void* GetMemory(memory_block block) {
	return block.memory;
}

exported_function bool IsValid(memory_block block) {
	return block.memory != Null && block.size != Null;
}

exported_function void SetInvalid(memory_block& block) {
	block.memory = Null;
	block.size = 0;
}
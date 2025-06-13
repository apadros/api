#include "apad_base_types.h"
#include "apad_debug_error.h"
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
	Assert(memory != Null);
	if(ErrorIsSet() == true)
		return;
	
  Assert(size > 0);
	if(ErrorIsSet() == true)
		return;
	
  for (memory_size it = 0; it < size; it++)
		((ui8*)memory)[it] = 0;
}

exported_function void CopyMemory(void* source, memory_size size, void* destination) {
  Assert(source != Null);
	if(ErrorIsSet() == true)
		return;
	
  Assert(size > 0);
	if(ErrorIsSet() == true)
		return;
	
  Assert(destination != Null);
	if(ErrorIsSet() == true)
		return;
	
	Assert(source != destination);
	if(ErrorIsSet() == true)
		return;
	
  ui8* ps = (ui8*)source;
  ui8* pd = (ui8*)destination;
  
  for (memory_size it = 0; it < size; it++)
    pd[it] = ps[it];
}

#include "apad_win32.h"
exported_function memory_block AllocateMemory(ui32 size) {
	void* memory = AllocateWin32Memory(size);
	if(ErrorIsSet() == true)
		return NullMemoryBlock;
		
	memory_block ret;
	ClearStruct(ret);
	ret.memory = memory;
	ret.size = size;
	
	return ret;
}

#include "apad_win32.h"
exported_function void FreeMemory(memory_block block) {
	FreeWin32Memory(block.memory);
}

exported_function void* GetMemory(memory_block block) {
	Assert(block.memory != Null);
	return block.memory;
}

exported_function bool IsValid(memory_block block) {
	return block.memory != Null && block.size != Null;
}

exported_function void SetInvalid(memory_block& block) {
	block.memory = Null;
	block.size = 0;
}

program_local int main() {
	return 0;
}
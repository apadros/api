#include <string.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

// ******************** Internal API start ******************** //

program_local void* AllocatedMemory; // Pool allocation of void pointers allocated as the OS level
program_local ui32  AllocatedMemoryLength;
#define 						BeginAllocatedMemoryLoop(_varID) { ForAll(AllocatedMemoryLength) { \
																											   void** _varID = (void**)AllocatedMemory + it;
#define 						BreakAllocatedMemoryLoop() 			 break
#define 						EndAllocatedMemoryLoop()   			 } }

program_local void ExitMemoryAPI() {
	FunctionStart(;);
	
	BeginAllocatedMemoryLoop(mem) {
		if(*mem != Null)
			Win32FreeMemory(*mem);
	}
	EndAllocatedMemoryLoop();
	
	FunctionEnd();
}

// ******************** Internal API end ******************** //

dll_export void Reset(memory_block& stack) {
	FunctionStart(;);
	if(stack.size > 0)
		Clear(stack.memory, stack.size);
  stack.size = 0;
	FunctionEnd();
}

dll_export void Clear(void* memory, ui32 size) {
	FunctionStart(;);
	AssertInternal(memory != Null);
  AssertInternal(size > 0);
	
	memset(memory, 0, size);
	FunctionEnd();
}

dll_export void Copy(void* source, ui32 size, void* destination) {
  FunctionStart(;);
	AssertInternal(source != Null);
  AssertInternal(size > 0);
  AssertInternal(destination != Null);
	AssertInternal(source != destination);
	
	memcpy(destination, source, size);
	FunctionEnd();
}

dll_export memory_block AllocateMemory(ui32 size) {
	FunctionStart(memory_block());
	
	void* memory = Win32AllocateMemory(size);
		
	// Init global table
	if(AllocatedMemory == Null) {
		AllocatedMemoryLength = 64;
		AllocatedMemory = Win32AllocateMemory(AllocatedMemoryLength * sizeof(void*));
		RegisterExitFunction(ExitMemoryAPI);
	}
	
	// Find space on global table and store pointer
	bool allocated = false;
	BeginAllocatedMemoryLoop(mem) {
		if(*mem == Null) {
			*mem = memory;
			allocated = true;
			BreakAllocatedMemoryLoop();
		}
	}
	EndAllocatedMemoryLoop();
	
	// Expand global table if needed and store new pointer if so
	if(allocated == false) {
		ui32 oldLength = AllocatedMemoryLength;
		
		// Expand table and copy over old memory
		ui32  newLength = oldLength * 2;
		void* newMemory = Win32AllocateMemory(newLength * sizeof(void*));
		Copy(AllocatedMemory, AllocatedMemoryLength * sizeof(void*), newMemory);
		Win32FreeMemory(AllocatedMemory);
		AllocatedMemory = newMemory;
		AllocatedMemoryLength = newLength;
		
		((void**)AllocatedMemory)[oldLength] = memory;
	}
	
	memory_block ret;
	ClearInstance(ret);
	ret.memory = memory;
	ret.size = size;
	
	FunctionEnd();
	return ret;
}

dll_export void Expand(memory_block& b) {
	FunctionStart(;);
	
	void* newMemory = Null;
	if(b.capacity != Null) {
		b.capacity *= 2;
		newMemory = Win32AllocateMemory(b.capacity);
		Copy(b.memory, b.size, newMemory);
	}
	else {
		b.size *= 2;
		newMemory = Win32AllocateMemory(b.size);
		Copy(b.memory, b.size / 2, newMemory);
	}
	
	Win32FreeMemory(b.memory);
	b.memory = newMemory;
	
	FunctionEnd();
}

dll_export void Free(memory_block& block) {
	FunctionStart(;);
	
	Free(block.memory);
	SetInvalid(block);
	
	FunctionEnd();
}

dll_export void Free(void* memory) {
	FunctionStart(;);
	
	Win32FreeMemory(memory);
	
	BeginAllocatedMemoryLoop(mem) {
		if(*mem == memory) {
			*mem = Null;
			BreakAllocatedMemoryLoop();
		}
	}
	EndAllocatedMemoryLoop();
	
	FunctionEnd();
}

dll_export bool IsValid(memory_block block) {
	FunctionStart(false);
	if(block.memory == Null)
		return false;
	
	if(block.capacity > 0)
		return block.size <= block.capacity;
	else
		return block.size > 0;
	
	FunctionEnd();
	return true;
}

dll_export void SetInvalid(memory_block& block) {
	FunctionStart(;);
	block.memory = Null;
	block.size = 0;
	block.capacity = 0;
	FunctionEnd();
}

dll_export memory_block AllocateStack(ui32 capacity) {
	FunctionStart(memory_block());
	
	if(capacity == Null)
		capacity = 1;
	auto block = AllocateMemory(capacity);
	block.capacity = block.size;
	block.size = 0;
	
	FunctionEnd();
	return block;
}

dll_export void* Insert(ui32 size, ui32 offset, memory_stack& stack) {
	FunctionStart(Null);
	AssertInternal(size > 0);
	AssertInternal(offset <= stack.size);
	
	// Push at the end in case stack needs to be reallocated
	Push(size, stack);
	
	// Move everything from offset up up by size
	// Do so manually since we're modifying the same memory we're reading from
	FromTo(stack.size - size, offset) {
		ui8* src  = (ui8*)stack.memory + it - 1;
		ui8* dest = (ui8*)stack.memory + it;
		*dest = *src;
	}
	
	void* ret = (ui8*)stack.memory + offset;
	Clear(ret, size);
	
	FunctionEnd();
	return ret;
}

#include "apad_maths.h"
dll_export void Remove(ui32 size, ui32 offset, memory_stack& stack) {
	FunctionStart(;);
	AssertInternal(size > 0);
	AssertInternal(offset < stack.size);
	AssertInternal(offset + size <= stack.size);
	
	ui32 sizeToMove = stack.size - (offset + size);
	
	ForAll(sizeToMove) { // Move manually since we're reading from and writing to the same memory block
		ui8* src = (ui8*)stack.memory + offset + size + it;
	  ui8* dest = (ui8*)stack.memory + offset + it;
		*dest = *src;
	}
	
	Clear((ui8*)stack.memory + stack.size - size, size);
	stack.size -= size;
	
	FunctionEnd();
}

dll_export void* Push(ui32 size, memory_block& stack) {
	FunctionStart(Null);
	AssertInternal(IsValid(stack));
	AssertInternal(size > 0);
	
	if(stack.size + size <= stack.capacity) { // If allocating within stack capacity
		void* ret = (ui8*)stack.memory + stack.size;
		stack.size += size;
		
		FunctionEnd();
		return ret;
	}
	else { // Else allocate new stack, copy contents over, then free old stack
		ui32 	newCapacity = stack.capacity;
		do 		newCapacity *= 2;
		while(stack.size + size > newCapacity);
	
		auto newStack = AllocateStack(newCapacity);
		
		if(stack.size > 0) // If == 0 it will trigger an error
			Push(stack.memory, stack.size, newStack);
		
		Free(stack);
		stack = newStack;
		
		FunctionEnd();
		return Push(size, stack);
	}
	
	FunctionEnd();
}

dll_export void* Push(void* data, ui32 size, memory_block& stack) {
	FunctionStart(Null);
  void* mem = Push(size, stack);
	Copy(data, size, mem);
	FunctionEnd();
	return mem;
}

dll_export memory_offset GetOffset(void* memory, memory_block& block) {
	FunctionStart(memory_offset());
	AssertInternal(IsValid(block) == true);
	AssertInternal(memory >= block.memory);
	if(block.capacity != Null)
		AssertInternal(memory < (ui8*)block.memory + block.capacity)
	else
		AssertInternal(memory < (ui8*)block.memory + block.size);
	
	memory_offset ret = {};
	ret.block = &block;
	ret.offset = (ui8*)memory - (ui8*)block.memory;
	
	FunctionEnd();
	return ret;
}

dll_export void* GetMemory(memory_offset offset) {
	FunctionStart(Null);
	AssertInternal(offset.block != Null);
	
	void* mem = (ui8*)offset.block->memory + offset.offset;
	
	FunctionEnd();
	return mem;
}

dll_export bool IsValid(memory_offset offset) {
	FunctionStart(false);
	
	bool ret = true;
	if(offset.block == Null)
		ret = false;
	else if(IsValid(*offset.block) == false)
		ret = false;
	else if(offset.block->capacity != Null && offset.offset >= offset.block->capacity)
		ret = false;
	else
		ret = offset.offset < offset.block->size;
	
	FunctionEnd();
	return ret;
}
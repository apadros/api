#include <string.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

// ******************** Internal API start ******************** //

// ******************** Internal API end ******************** //

dll_export void ResetStack(memory_block& stack) {
	FunctionStart(;);
	ClearMemory(stack.memory, stack.size);
  stack.size = 0;
	FunctionEnd();
}

dll_export void ClearMemory(void* memory, ui32 size) {
	FunctionStart(;);
	AssertInternal(memory != Null);
  AssertInternal(size > 0);
	
	memset(memory, 0, size);
	FunctionEnd();
}

dll_export void CopyMemory(void* source, ui32 size, void* destination) {
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
		
	memory_block ret;
	ClearStruct(ret);
	ret.memory = memory;
	ret.size = size;
	
	FunctionEnd();
	return ret;
}

dll_export void FreeMemory(memory_block& block) {
	FunctionStart(;);
	Win32FreeMemory(block.memory);
	ClearStruct(block);
	FunctionEnd();
}

dll_export void* GetMemory(memory_block block) {
	FunctionStart(Null);
	AssertInternal(block.memory != Null);
	FunctionEnd();
	return block.memory;
}

dll_export bool IsValid(memory_block block) {
	if(block.memory == Null)
		return false;
	
	if(block.capacity > 0)
		return block.size <= block.capacity;
	else
		return block.size > 0;
	
	return true;
}

dll_export void SetInvalid(memory_block& block) {
	block.memory = Null;
	block.size = 0;
	block.capacity = 0;
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

dll_export void FreeStack(memory_block& stack) {
	FunctionStart(;);
	AssertInternal(IsValid(stack));
	
	FreeMemory(stack);
	
	FunctionEnd();
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
	ClearMemory(ret, size);
	
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
	
	ClearMemory((ui8*)stack.memory + stack.size - size, size);
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
		
		FreeStack(stack);
		stack = newStack;
		
		FunctionEnd();
		return Push(size, stack);
	}
	
	FunctionEnd();
}

dll_export void* Push(void* data, ui32 size, memory_block& stack) {
	FunctionStart(Null);
  void* mem = Push(size, stack);
	CopyMemory(data, size, mem);
	FunctionEnd();
	return mem;
}
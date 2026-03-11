#include <string.h>
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

// ******************** Internal API start ******************** //

// ******************** Internal API end ******************** //

dll_export void ResetStack(memory_block& stack) {
	ClearMemory(stack.memory, stack.size);
  stack.size = 0;
}

dll_export void ClearMemory(void* memory, ui32 size) {
	AssertRet(memory != Null);
  AssertRet(size > 0);
	
	memset(memory, 0, size);
}

dll_export void CopyMemory(void* source, ui32 size, void* destination) {
  AssertRet(source != Null);
  AssertRet(size > 0);
  AssertRet(destination != Null);
	AssertRet(source != destination);
	
	memcpy(destination, source, size);
}

dll_export memory_block AllocateMemory(ui32 size) {
	void* memory = Win32AllocateMemory(size);
	if(GlobalErrorIsSet() == true)
		return NullMemoryBlock;
		
	memory_block ret;
	ClearStruct(ret);
	ret.memory = memory;
	ret.size = size;
	
	return ret;
}

dll_export void FreeMemory(memory_block& block) {
	Win32FreeMemory(block.memory);
	ClearStruct(block);
}

dll_export void* GetMemory(memory_block block) {
	AssertRetType(block.memory != Null, Null);
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
	if(capacity == Null)
		capacity = 1;
	auto block = AllocateMemory(capacity);
	block.capacity = block.size;
	block.size = 0;
	return block;
}

dll_export void FreeStack(memory_block& stack) {
	AssertRet(IsValid(stack));
	
	FreeMemory(stack);
}

dll_export void* Push(ui32 size, memory_block& stack) {
	AssertRetType(IsValid(stack), Null);
	AssertRetType(size > 0, Null);
	
	if(stack.size + size <= stack.capacity) { // If allocating within stack capacity
		void* ret = (ui8*)stack.memory + stack.size;
		stack.size += size;
		return ret;
	}
	else { // Else allocate new stack, copy contents over, then free old stack
		ui32 	newCapacity = stack.capacity;
		do 		newCapacity *= 2;
		while(stack.size + size > newCapacity);
	
		auto newStack = AllocateStack(newCapacity);
		if(GlobalErrorIsSet() == true)
			return Null;
		
		if(stack.size > 0) { // If == 0 it will trigger an error
			Push(stack.memory, stack.size, newStack);
			if(GlobalErrorIsSet() == true)
				return Null;
		}
		
		FreeStack(stack);
		if(GlobalErrorIsSet() == true)
			return Null;
		
		stack = newStack;
		
		return Push(size, stack);
	}
	
	InvalidCodePath;
	return Null;
}

dll_export void* Push(void* data, ui32 size, memory_block& stack) {
  void* mem = Push(size, stack);
	if(GlobalErrorIsSet() == true)
		return Null;
	CopyMemory(data, size, mem);
	if(GlobalErrorIsSet() == true)
		return Null;
	return mem;
}
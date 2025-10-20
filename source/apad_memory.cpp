#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_win32.h"

// ******************** Local API start ******************** //

ui32 maxStackCapacity = MiB(1);

// ******************** Local API end ******************** //

exported_function void ClearMemory(void* memory, ui32 size) {
	AssertRet(memory != Null);
	
  AssertRet(size > 0);
	
  for (ui32 it = 0; it < size; it++)
		((ui8*)memory)[it] = 0;
}

exported_function void CopyMemory(void* source, ui32 size, void* destination) {
  AssertRet(source != Null);
  AssertRet(size > 0);
  AssertRet(destination != Null);
	AssertRet(source != destination);
	
  ui8* ps = (ui8*)source;
  ui8* pd = (ui8*)destination;
  
  for (ui32 it = 0; it < size; it++)
    pd[it] = ps[it];
}

exported_function memory_block AllocateMemory(ui32 size) {
	void* memory = Win32AllocateMemory(size);
	if(ErrorIsSet() == true)
		return NullMemoryBlock;
		
	memory_block ret;
	ClearStruct(ret);
	ret.memory = memory;
	ret.size = size;
	
	return ret;
}

exported_function void FreeMemory(memory_block& block) {
	Win32FreeMemory(block.memory);
	ClearStruct(block);
}

exported_function void* GetMemory(memory_block block) {
	AssertRetType(block.memory != Null, Null);
	return block.memory;
}

exported_function bool IsValid(memory_block block) {
	AssertRetType(block.memory != Null, false);
	if(block.capacity > 0)
		AssertRetType(block.size <= block.capacity, false)
	else
		AssertRetType(block.size != 0, false)
	return true;
}

exported_function void SetInvalid(memory_block& block) {
	block.memory = Null;
	block.size = 0;
	block.capacity = 0;
}

exported_function memory_block AllocateStack(ui32 capacity) {
	AssertRetType(capacity <= maxStackCapacity, memory_block());
	auto block = AllocateMemory(capacity);
	block.capacity = block.size;
	block.size = 0;
	return block;
}

exported_function void FreeStack(memory_block& stack) {
	AssertRet(IsValid(stack));
	
	FreeMemory(stack);
}

exported_function void* PushMemory(ui32 size, memory_block& stack) {
	AssertRetType(IsValid(stack), Null);
  
	if(size + stack.size <= stack.capacity) { // If allocating within stack capacity
		void* ret = (ui8*)stack.memory + stack.size;
		stack.size += size;
		return ret;
	}
	else { // Else allocate new stack, copy contents over, then free old stack
		// Set a constraint to avoid a potential recursive bug
	  AssertRetType(stack.capacity * 2 <= maxStackCapacity, Null);
		
		auto  newStack = AllocateStack(stack.capacity * 2);
		newStack.size = stack.size;
		void* retMem = PushMemory(size, newStack); // Recursive call until right capacity found
		if(ErrorIsSet() == true)
			return Null;
		CopyMemory(stack.memory, stack.size, retMem);
		FreeMemory(stack);
		stack = newStack;
		return retMem;
	}
}

exported_function void* PushData(void* data, ui32 size, memory_block& stack) {
  void* mem = PushMemory(size, stack);
	CopyMemory(data, size, mem);
	return mem;
}

exported_function void SetMaxStackCapacity(ui32 capacity) {
	maxStackCapacity = capacity;
}
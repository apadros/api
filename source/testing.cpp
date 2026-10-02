#include <assert.h>
#include "apad_memory.h"
void RunMemoryTest() {
	// KiB(), MiB() & GiB()
	{
		ui32 value1 = KiB(2);
		assert(value1 == 2 * 1024);
		
		value1 = MiB(4);
		assert(value1 == 4 * 1024 * 1024);
		
		ui64 value2 = GiB(8);
		assert(value == 8 * 1024 * 1024 * 1024);
	}
	
	// MovePtr(), CastMemMovePtr() & ReadMemMovePtr()
	{
		ui32 ints[] = { 0xFF, 0xA, 0x9, 0xE, 0xD, 10 };
		ui32* p = ints;
		MovePtr(p, sizeof(ints[0]));
		assert(*p == ints[1]);
		MovePtr(p, sizeof(ints[0]));
		CastMemMovePtr(p, f32);
		assert(*p == ints[3]);
		MovePtr(p, sizeof(ints[0]));
		ui32 read = ReadMemMovePtr(p, decltype(ints[0]));
		assert(read == ints[4]);
		assert(p == ints + 5);
	}
	
	// Clear(), ClearInstance(), Copy() & CopyInstance()
	{
		ui32 ints[] = { 0xFF, 0xA, 0x9, 0xE };
		ui32* p = ints + 1;
		Clear(p, sizeof(ints[0]);
		assert(ints[1] == 0);
		ClearInstance(ints[0]);
		assert(ints[0] == 0);
		Copy(ints + 3, sizeof(ints[3]), ints);
		assert(ints[0] == ints[3]);
		CopyInstance(ints[2], ints[1]);
		assert(ints[1] == ints[2]);
	}
	
	// memory_block API global APAD API memory
	{
		extern void* AllocatedMemory;
		extern ui32  AllocatedMemoryLength;
		
		auto block = AllocateMemory(32);
		assert(block.memory != Null && block.size == 32);
		
		assert(IsValid(block) == true);
		
		assert(((void**)AllocatedMemory)[0] == block.memory);
		
		auto copy = block;
		assert(copy.memory == block.memory && copy.size == block.size);
		SetInvalid(copy);
		assert(copy.memory == Null && copy.size == Null);
		
		auto block2 = AllocateMemory(8);
		assert(block2.memory != Null && block.size == 8);
		assert(((void**)AllocatedMemory)[1] == block2.memory);
		
		Free(block);
		assert(IsValid(block) == false);
		assert(block.memory == Null && block.size == 0);
		assert(((void**)AllocatedMemory)[0] == Null);
		
		Free(((void**)AllocatedMemory)[1]);
		assert(((void**)AllocatedMemory)[1] == Null);
	}
	
	// memory_offset API
	{
		auto block = AllocateMemory(16);
		assert(IsValid(block) == true);
		
		ui8 bytes = 8;
		void* mem = (ui8*)memory.memory + bytes;
		auto offset = GetOffset(mem, memory);
		assert(offset.block == &block);
		assert(offset.offset == bytes);
		assert(IsValid(offset) == true);
		
		void* offsetMem = GetMemory(offset);
		assert(offsetMem == mem);
		
		SetInvalid(offset);
		assert(offset.block == Null && offset.offset == Null);
	}
	
	// memory_stack API
	{
		auto stack = AllocateStack();
		assert(stack.memory.memory != Null);
		assert(stack.memory.size == 1); // Stack capacity
		assert(stack.size == 0);
		assert(IsValid(stack) == true);
		
		ui32 u1 = 0xFE56A;
		ui16 u2 = 0xA7;
		
		// Push(), GetMemory() & GetCapacity()
		void* mem = Push(&u1, sizeof(u1), stack);
		assert(GetMemory(stack) != Null && GetMemory(stack) == mem);
		assert(stack.size == sizeof(u));
		assert(*((decltype(u1)*)mem) == u1);
		assert(stack.memory.size == 4) // Test expanded capacity as a result of pushing beyond previous one
		assert(stack.memory.size == stack.size);
		assert(GetCapacity(stack) == sizeof(u1));
		
		// More Push() calls
		mem = Push(&u2, sizeof(u2), stack);
		assert(mem == (ui8*)stack.memory + sizeof(u1));
		assert(stack.size == sizeof(u1) + sizeof(u2));
		assert(GetCapacity(stack) == sizeof(u1) * 2) // Test expanded capacity as a result of pushing beyond previous one
		Push(&u2, sizeof(u2), stack);
		assert(stack.size == sizeof(u1) + sizeof(u2) * 2);
		assert(GetCapacity(stack) == stack.size) // Test expanded capacity as a result of pushing beyond previous one
		
		// Pop()
		Pop(sizeof(u2), stack);
		assert(stack.size == sizeof(u1) + sizeof(u2));
		assert(GetCapacity(stack) == sizeof(u1) + sizeof(u2) * 2);
		
		// Insert()
		ui32 u3 = 0xAD;
		mem = Insert(sizeof(u3), sizeof(u1), stack);
		assert(mem == (ui8*)GetMemory(stack) + sizeof(u1));
		assert(GetCapacity(stack) == sizeof(u1) * 4);
		assert(stack.size == sizeof(u1) + sizeof(u3) + sizeof(u2));
		
		// Remove() & test whether contents of stack were moved down properly
		Remove(sizeof(u3), sizeof(u1), stack);
		assert(GetCapacity(stack) == sizeof(u1) * 4); // Same capacity as before
		assert(stack.size == sizeof(u1) + sizeof(u2));
		mem = GetMemory(stack);
		ui32 test1 = ReadMemMovePtr(mem, ui32); 
		ui16 test2 = ReadMemMovePtr(mem, ui16);
		assert(test1 == u1 && test2 == u2);
		
		// Reset()
		Reset(stack);
		assert(IsValid(stack.memory) == true && GetMemory(stack) != Null && stack.size == 0 && GetCapacity(stack) != Null);
		
		// Free()
		Free(stack);
		assert(IsValid(stack.memory) == false && GetMemory(stack) == Null && stack.size == 0 && GetCapacity(stack) == Null);
		
		// Allocation with requested size
		ui32 capacity = 16;
		stack = AllocateStack(capacity);
		assert(IsValid(stack) == true && GetCapacity(stack) == capacity && stack.size == 0);
		Free(stack);
	}
	
	// memory_pool @WIP - Finish
	{
		memory_block element;
		ui16 				 count = 4;
		auto pool = AllocatePool(sizeof(element), count);
		assert(pool.elementSize == sizeof(element));
		ui32 capacity = pool.memory.size;
		assert(capacity == sizeof(element) * count);
		
		// @TODO - Test expansion when allocating beyond limit
	}
}
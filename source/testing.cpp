#include <assert.h>
#include "apad_intrinsics.h"
#include "apad_memory.h"
dll_import void* AllocatedMemory;
dll_import ui32  AllocatedMemoryLength;
	
void RunMemoryTest() {
	// KiB(), MiB() & GiB()
	{
		ui32 value1 = KiB(2);
		assert(value1 == 2 * 1024);
		
		value1 = MiB(4);
		assert(value1 == 4 * 1024 * 1024);
		
		ui64 value2 = GiB(8);
		assert(value2 == 8 * 1024 * 1024 * 1024);
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
		ui32 read = ReadMemMovePtr(p, ui32);
		assert(read == ints[4]);
		assert(p == ints + 5);
	}
	
	// Clear(), ClearInstance(), Copy() & CopyInstance()
	{
		ui32 ints[] = { 0xFF, 0xA, 0x9, 0xE };
		ui32* p = ints + 1;
		Clear(p, sizeof(ints[0]));
		assert(ints[1] == 0);
		ClearInstance(ints[0]);
		assert(ints[0] == 0);
		Copy(ints + 3, sizeof(ints[3]), ints);
		assert(ints[0] == ints[3]);
		CopyInstance(ints[2], ints + 1);
		assert(ints[1] == ints[2]);
	}
		
	// memory_block API global APAD API memory
	{
		auto block = AllocateMemory(32);
		assert(block.memory != Null && block.size == 32);
		
		assert(IsValid(block) == true);
		
		assert(((void**)AllocatedMemory)[0] == block.memory);
		
		auto copy = block;
		assert(copy.memory == block.memory && copy.size == block.size);
		SetInvalid(copy);
		assert(copy.memory == Null && copy.size == Null);
		
		auto block2 = AllocateMemory(8);
		assert(block2.memory != Null && block2.size == 8);
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
		void* mem = (ui8*)block.memory + bytes;
		auto offset = GetOffset(mem, block);
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
		assert(stack.size == sizeof(u1));
		assert(*((decltype(u1)*)mem) == u1);
		assert(stack.memory.size == sizeof(u1)); // Test expanded capacity as a result of pushing beyond previous one
		assert(stack.memory.size == stack.size);
		assert(GetCapacity(stack) == sizeof(u1));
		
		// Memory layout: u1 (ui32)
		
		// More Push() calls
		mem = Push(&u2, sizeof(u2), stack);
		assert(mem == (ui8*)GetMemory(stack) + sizeof(u1));
		assert(stack.size == sizeof(u1) + sizeof(u2));
		assert(GetCapacity(stack) == sizeof(u1) * 2); // Test expanded capacity as a result of pushing beyond previous one
		Push(&u2, sizeof(u2), stack);
		assert(stack.size == sizeof(u1) + sizeof(u2) * 2);
		assert(GetCapacity(stack) == stack.size); // Test expanded capacity as a result of pushing beyond previous one
		
		// Memory layout: u1 (ui32), u2 (ui16), u2 (ui16)
		
		// Pop()
		Pop(sizeof(u2), stack);
		assert(stack.size == sizeof(u1) + sizeof(u2));
		assert(GetCapacity(stack) == sizeof(u1) + sizeof(u2) * 2);
		{
			mem = GetMemory(stack);
			ui32 test1 = ReadMemMovePtr(mem, ui32); 
			ui16 test2 = ReadMemMovePtr(mem, ui16);
			assert(test1 == u1);
			assert(test2 == u2);
		}
		
		// Memory layout: u1 (ui32), u2 (ui16)
		
		// Insert()
		ui32 u3 = 0xAD;
		mem = Insert(sizeof(u3), sizeof(u1), stack);
		assert(mem == (ui8*)GetMemory(stack) + sizeof(u1));
		assert(GetCapacity(stack) == sizeof(u1) * 4);
		assert(stack.size == sizeof(u1) + sizeof(u3) + sizeof(u2));
		{
			mem = GetMemory(stack);
			ui32 test1 = ReadMemMovePtr(mem, ui32); 
			ui32 test2 = ReadMemMovePtr(mem, ui32);
			ui16 test3 = ReadMemMovePtr(mem, ui16);
			assert(test1 == u1);
			assert(test2 == 0);
			assert(test3 == u2);
		}
		
		// Memory layout: u1 (ui32), u3 (ui32), u2 (ui16)
		
		// Remove() & test whether contents of stack were moved down properly
		Remove(sizeof(u3), sizeof(u1), stack);
		assert(GetCapacity(stack) == sizeof(u1) * 4); // Same capacity as before
		assert(stack.size == sizeof(u1) + sizeof(u2));
		{
			mem = GetMemory(stack);
			ui32 test1 = ReadMemMovePtr(mem, ui32); 
			ui16 test2 = ReadMemMovePtr(mem, ui16);
			assert(test1 == u1);
			assert(test2 == u2);
		}
		
		// Reset()
		Reset(stack);
		assert(IsValid(stack.memory) == true);
		assert(GetMemory(stack) != Null);
		assert(stack.size == 0);
		assert(GetCapacity(stack) != Null);
		
		// Free()
		Free(stack);
		assert(IsValid(stack.memory) == false);
		assert(stack.memory.memory == Null); // Do this instead of calling GetMemory() since latter will assert internal memory to be valid
		assert(stack.size == 0);
		assert(stack.memory.size == Null); // Do this instead of calling GetCapacity() since latter will assert internal memory to be valid
		
		// Allocation with requested size
		ui32 capacity = 16;
		stack = AllocateStack(capacity);
		assert(IsValid(stack) == true && GetCapacity(stack) == capacity && stack.size == 0);
		Free(stack);
	}
	
	// memory_pool
	{
		// AllocatePool()
		memory_block element;
		ui16 				 count = 2;
		auto pool = AllocatePool(sizeof(element), count);
		assert(pool.elementSize == sizeof(element));
		ui32 capacity = pool.memory.size;
		assert(capacity == (sizeof(b8) + sizeof(element)) * count);
		assert(IsValid(pool.memory) == true);
		
		// Allocate()
		void* mem = Allocate(pool);
		assert(mem == (ui8*)pool.memory.memory + sizeof(b8));
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == true);
		
		// Various Allocate() calls & capacity expansion
		mem = Allocate(pool);
		assert(mem == (ui8*)pool.memory.memory + sizeof(b8) + sizeof(element) + sizeof(b8));
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == true);
		
		mem = Allocate(pool);
		capacity = pool.memory.size;
		assert(capacity == (sizeof(b8) + sizeof(element)) * count * 2);
		assert(mem == (ui8*)pool.memory.memory + (sizeof(b8) + sizeof(element)) * 2 + sizeof(b8));
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == true);
		
		// Deallocate() & reallocation of freed memory
		mem = (ui8*)pool.memory.memory + sizeof(b8) + sizeof(element) + sizeof(b8);
		Deallocate(mem, pool);
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == false);
		mem = Allocate(pool);
		assert(mem == (ui8*)pool.memory.memory + sizeof(b8) + sizeof(element) + sizeof(b8));
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == true);
		
		// Allocate(void*, ui16 size, memory_pool&) - test allocation and copying of data
		element.size = 0xFA;
		auto offset = Allocate(&element, sizeof(element), pool);
		mem = GetMemory(offset);
		assert(mem == (ui8*)pool.memory.memory + (sizeof(b8) + sizeof(element)) * 3 + sizeof(b8));
		auto* temp = (decltype(element)*)mem;
		assert(temp->size == element.size);
		mem = (ui8*)mem - sizeof(b8);
		assert(*((b8*)mem) == true);
		
		// Deallocate(memory_offset, memory_pool)
		Deallocate(offset, pool);
		assert(*((b8*)mem) == false);
	}
	
	 // @WIP - Run into issues at the very end with ExitMemoryAPI() in apad_memory.cpp
}
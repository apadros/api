#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // For system()
#include "apad_intrinsics.h"
#include "apad_file.h"
#include "apad_memory.h"
#include "apad_string.h"

void RunStringAPITest() {
	// GetLength(), ConvertStringToLowerCase() & Free()
	{
		char* string = "HeL_Lo1";
		assert(GetLength(string) == 7);
		string = ConvertStringToLowerCase(string, Null);
		assert(string[0] == 'h' && string[1] == 'e' && string[2] == 'l' && 
					 string[3] == '_' && string[4] == 'l' && string[5] == 'o' && string[6] == '1');
		assert(AreEqual(string, "hel_lo1") == true);
		Free(string);
	}
	
	// All ToString() overloads, StringToInt()
	{
		si8 s8 = -5;
		char* string = ToString(s8);
		assert(GetLength(string) == 2);
		assert(string[0] == '-' && string[1] == '5');
		assert(AreEqual(string, "-5") == true);
		Free(string);
		
		ui8 u8 = 10;
		string = ToString(u8);
		assert(GetLength(string) == 2);
		assert(AreEqual(string, "10") == true);
		Free(string);
		
		si16 s16 = -908;
		string = ToString(s16);
		assert(GetLength(string) == 4);
		assert(AreEqual(string, "-908") == true);
		Free(string);
		
		ui16 u16 = 407;
		string = ToString(u16);
		assert(GetLength(string) == 3);
		assert(AreEqual(string, "407") == true);
		Free(string);
		
		si32 s32 = -68575;
		string = ToString(s32);
		assert(GetLength(string) == 6);
		assert(AreEqual(string, "-68575") == true);
		Free(string);
		
		ui32 u32 = 71420;
		string = ToString(u32);
		assert(GetLength(string) == 5);
		assert(AreEqual(string, "71420") == true);
		Free(string);
		
		si64 s64 = -55'765'819'430;
		string = ToString(s64);
		assert(GetLength(string) == 12);
		assert(AreEqual(string, "-55765819430") == true);
		Free(string);
		
		ui64 u64 = 102'698'610'058;
		string = ToString(u64);
		assert(GetLength(string) == 12);
		assert(AreEqual(string, "102698610058") == true);
		Free(string);
		
		f32  f   = 3.218;
		string = ToString(f);
		assert(GetLength(string) == 4);
		assert(AreEqual(string, "3.22") == true);
		Free(string);
		
		f64  d   = 6.159;
		string = ToString(d);
		assert(GetLength(string) == 4);
		assert(AreEqual(string, "6.16") == true);
		Free(string);
		
		auto i = StringToInt("-3812", Null);
		assert(i == -3812);
	}
	
	// IsLetter(), IsWord(), IsNumber() & IsNumber()
	{
		assert(IsLetter('t') == true);
		assert(IsNumber('7') == true);
		assert(IsWord("hello", Null) == true);
		assert(IsNumber("27", Null) == true);
	}
	
	
	printf("\nString API testing OK\n");
}

void RunFileAPITest() {
	// Create new file and add sample string
	system("if exist temp ( rmdir temp /s /q )");
	system("mkdir temp");
	system("echo hello>> temp/test_file.txt");
	
	const char* path = "temp/test_file.txt";
	
	// GetFileNameAndExtension(), GetFileExtension
	{
		const char* nameExtension = GetFileNameAndExtension(path);
		ui16 pathLength = 18;
		for(ui8 i = 0; i < pathLength - 5; i++)
			assert(nameExtension[i] == path[i + 5]);
		
		const char* extension = GetFileExtension(path);
		for(ui8 i = 0; i < 3; i++)
			assert(extension[i] == path[i + 15]);
		
		// Test without the slash
		const char* path2 = "new_test_file.txt";
		nameExtension = GetFileNameAndExtension(path2);
		for(ui8 i = 0; i < 17; i++)
			assert(nameExtension[i] == path2[i]);
	}
		
	// FileExists(), LoadFile(), IsValid()
	assert(FileExists(path) == true);
	auto f = LoadFile(path);
	assert(IsValid(f) == true);
	
	// GetSize(), GetMemory()
	{
		ui32 size = GetSize(f);
		assert(size == 7); // Cause echo >> .txt will add a newline
		void* mem = GetMemory(f);
		const char* string = (const char*)mem;
		assert(string[0] == 'h' && string[1] == 'e' && string[2] == 'l' && 
					 string[3] == 'l' && string[4] == 'o');
	}
	
	// FreeFile(), DeleteFile()
	FreeFile(f);
	assert(IsValid(f) == false);
	DeleteFile(path);
	assert(FileExists(path) == false);
	
	// SaveFile() + overload
	{
		const char* string = "world2";
		SaveFile((void*)string, 6, path);
		assert(FileExists(path) == true);
		f = LoadFile(path);
		assert(IsValid(f) == true);
		const char* mem = (const char*)GetMemory(f);
		assert(GetSize(f) == 6);
		assert(mem[0] == 'w' && mem[1] == 'o' && mem[2] == 'r' && 
					 mem[3] == 'l' && mem[4] == 'd' && mem[5] == '2');
					 
		const char* newPath = "test_file_2.txt";
		assert(FileExists(newPath) == false);
		SaveFile(f, newPath);
		assert(FileExists(newPath) == true);
				
		FreeFile(f);
		DeleteFile(newPath);
	}
	
	// ParseLine(), IsValid(), GetDataElement() & Free()
	{
		const char* string = "56 hello 8.9\n\"more data\" 77";
		ui8 				length = 28;
		SaveFile((void*)string, length, path);
		assert(FileExists(path) == true);
		file f = LoadFile(path);
		assert(IsValid(f) == true);
		assert(GetSize(f) == length);
		
		ui32 readIndex = 0;
		auto line = ParseLine(f, readIndex);
		assert(line.count == 3);
		assert(IsValid(line) == true);
		char* element = GetDataElement(line, 2);
		assert(element[0] == '8' && element[1] == '.' && element[2] == '9' && element[3] == '\0');
		Free(line);
		assert(IsValid(line) == false);
		assert(line.count == Null);
		
		// Test parsing of multiple lines
		line = ParseLine(f, readIndex);
		assert(line.count == 2);
		element = GetDataElement(line, 1);
		assert(element[0] == '7' && element[1] == '7');
		Free(line);
		assert(IsValid(line) == false);
		assert(line.count == Null);
		
		Free(f);
		DeleteFile(path);
	}
	
	// CreateFile(), Free(), WriteToFile() & overload
	{
		auto file = CreateFile();
		assert(IsValid(file) == true);
		
		const char* string = "hello world ";
		WriteToFile((char*)string, file);
		
		string = "\"new string\"";
		WriteToFile((char*)string, file);
		
		ui32 i = 90;
		WriteToFile(&i, sizeof(i), file);
		
		ui32 readIndex = 0;
		auto line = ParseLine(file, readIndex);
		assert(line.count == 4); // Hello and world will be considered 2 separate data elements
		char* element = GetDataElement(line, 2);
		assert(element[0] == 'n' && element[1] == 'e' && element[2] == 'w' && 
					 element[3] == ' ' && element[4] == 's');
		Free(line);
		
		Free(file);
	}
	
	system("rmdir temp /s /q");
	
	printf("\nFile API testing OK\n");
}
	
void RunMemoryAPITest() {
	dll_import void* AllocatedMemory;
	dll_import ui32  AllocatedMemoryLength;
	
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
	
	// At this piont AllocatedMemory[0] != Null
	
	// memory_stack API
	{
		// AllocateStack()
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
		assert(((void**)AllocatedMemory)[1] == pool.memory.memory);
		
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
		// At this point we've expanded inner memory and thus allocated a new block globally
		assert(((void**)AllocatedMemory)[1] == Null);
		assert(((void**)AllocatedMemory)[2] == pool.memory.memory);
		
		// Deallocate() & reallocation of deallocated memory
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
		
		// Free()
		Free(pool);
		assert(IsValid(pool.memory) == false);
		assert(pool.elementSize == Null);
		assert(((void**)AllocatedMemory)[2] == Null);
	}
		
	printf("\nMemory API testing OK\n");
}
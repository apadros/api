#ifdef CopyMemory
#undef CopyMemory
#endif

#ifndef APAD_MEMORY_H
#define APAD_MEMORY_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"

// ******************** Generic ******************** //

#define 							 KiB(value) ((value) * 1024)
#define 							 MiB(value) (KiB(value) * 1024)
#define 							 GiB(value) (MiB(value) * 1024)

#define 							 MovePtr(_ptr, _bytes) (_ptr) = (decltype(_ptr))((ui8*)(_ptr) + (_bytes))
#define 							 CastMemMovePtr(_mem, _dataType) ((_dataType*)(_mem)); MovePtr(_mem, sizeof(_dataType))
#define 							 ReadMemMovePtr(_mem, _dataType) *CastMemMovePtr(_mem, _dataType)

imported_function void ClearMemory(void* memory, ui32 size);
#define 							 ClearStruct(_s) ClearMemory(&(_s), sizeof(_s))
imported_function void CopyMemory(void* source, ui32 size, void* destination);

// ******************** Memory blocks ******************** //

struct memory_block {
  void* memory;
  ui32  size;
	ui32  capacity; // Stack functionality, will == 0 if not used this way
};
#define NullMemoryBlock memory_block()

imported_function memory_block AllocateMemory(ui32 size);
imported_function void*        GetMemory(memory_block block);
imported_function void         FreeMemory(memory_block& block); // Clears block afterwards
imported_function bool         IsValid(memory_block block);
imported_function void         SetInvalid(memory_block& block);

// ******************** Stack ******************** //

imported_function void         SetMaxStackCapacity(ui32 capacity); // For any single stack
imported_function memory_block AllocateStack(ui32 capacity);
imported_function void 				 PushData(void* data, ui32 size, memory_block& stack);
// Will allocate new stacks recursively if the requested size is larger 
// than the original stack's capacity until the right capacity is found
imported_function void* 			 PushMemory(ui32 size, memory_block& stack);
imported_function void 				 FreeStack(memory_block& stack);

#endif
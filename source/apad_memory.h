#ifdef CopyMemory
#undef CopyMemory
#endif

#ifndef APAD_MEMORY_API_H
#define APAD_MEMORY_API_H

#include "apad_intrinsics.h"


// ******************** Background ******************** //

typedef ui32 memory_size;
typedef si32 memory_offset;
typedef ui16 memory_block_id;

#define NullMemoryBlockID Null
#define MemoryBlockIDMax  UI16Max

struct memory_block {
  void*       memory;
  memory_size size;
};
#define NullMemoryBlock memory_block()

struct memory_stack {
  void*       memory;
  memory_size size;
  memory_size capacity;
};

#define KB(value) 		 (value * 1024)
#define MB(value) 		 (KB(value) * 1024)
#define ClearStruct(_s) ClearMemory(&(_s), sizeof(_s))

imported_function void ClearMemory(void* memory, memory_size size);
imported_function void CopyMemory(void* source, memory_size size, void* destination);

// ******************** Allocation & Deallocation ******************** //

imported_function memory_block AllocateMemory(memory_size size, b8 clear);
imported_function void*        GetMemory(memory_block block);
imported_function void         FreeMemory(memory_block block);
imported_function bool         IsValid(memory_block block);
imported_function void         SetInvalid(memory_block& block);

// ******************** Casting and reading ******************** //

#define MovePtr(_ptr, _bytes) (_ptr) = (decltype(_ptr))((ui8*)(_ptr) + (_bytes))
#define CastMemMovePtr(_mem, _dataType) ((_dataType*)(_mem)); MovePtr(_mem, sizeof(_dataType))
#define ReadMemMovePtr(_mem, _dataType) *CastMemMovePtr(_mem, _dataType)

#endif
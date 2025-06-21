#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

imported_function ui16 GetStringLength(const char*s, bool includeEOS);

// ******************** Number conversion ******************** //

struct short_string {
	char string[32];
};

imported_function short_string I64ToString(si64 i);
															 // Return limited to 2 decimal places without rounding
imported_function short_string F32ToString(f32 f);
imported_function const char*  PushString(const char* string, memory_block& stack);

#endif
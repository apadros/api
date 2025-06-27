#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Number conversion ******************** //

struct short_string {
	char string[32];
};

imported_function short_string ToString(si8 i);
imported_function short_string ToString(ui8 i);
imported_function short_string ToString(si16 i);
imported_function short_string ToString(ui16 i);
imported_function short_string ToString(si32 i);
imported_function short_string ToString(ui32 i);
imported_function short_string ToString(si64 i);
imported_function short_string ToString(ui64 i);
															 // Return limited to 2 decimal places without rounding
imported_function short_string ToString(f32 f);
															 // Return limited to 2 decimal places without rounding
imported_function short_string ToString(f64 f);

// ******************** Others ******************** //

imported_function bool 				ContainsAnySubstring(const char* string, const char** substrings, ui8 length);
imported_function const char* FindSubstring(const char*sub, const char*string);
imported_function ui16 				GetStringLength(const char*s, bool includeEOS);
imported_function const char* PushString(const char* string, bool includeEOS, memory_block& stack);
imported_function bool 				StringsAreEqual(const char* s1, const char* s2);

#endif
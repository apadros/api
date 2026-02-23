#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Conversions ******************** //

imported_function void 				ConvertStringToLowerCase(char* s);

imported_function const char* ToString(si8 i);
imported_function const char* ToString(ui8 i);
imported_function const char* ToString(si16 i);
imported_function const char* ToString(ui16 i);
imported_function const char* ToString(si32 i);
imported_function const char* ToString(ui32 i);
imported_function const char* ToString(si64 i);
imported_function const char* ToString(ui64 i);
															// Return limited to 2 decimal places without rounding
imported_function const char* ToString(f32 f);
															// Return limited to 2 decimal places without rounding
imported_function const char* ToString(f64 f);

imported_function si32 	 StringToInt(const char* string);

// ******************** Others ******************** //

imported_function bool IsLetter(char c);
imported_function bool IsNumber(char c);
imported_function bool IsWhitespace(char c); // Space, horizontal & vertical tabs, carriage return & newline

															// Allocates string on API global memory.
															// Will automatically add a null-char if target length does not contain one.
imported_function const char* AllocateString(const char* s, ui16 length /* Set to Null to copy until and including the null-char */ );

															// @TEST
imported_function const char* Concatenate(ui8 count, ... /* All args must be char* */);
imported_function bool 				ContainsAnySubstring(const char* string, const char** substrings, ui8 subsLength);
															// Set srcLength -1 to copy entire source including the null-character
imported_function void 				CopyString(const char* source, si16 srcLength, char* destination, ui16 destLength);
															// Allocates a copy on the heap.
															// If length > string length extraction will stop after the null-character.
															// @TEST
imported_function const char* ExtractSubstring(const char* string, ui8 length /* Set to Null to extract until the null-character */);
imported_function const char* FindSubstring(const char* sub, const char* string);
															// Will return the length wihtout the null-character
imported_function ui16 				GetStringLength(const char*s);
imported_function const char* PushString(const char* string, bool addEOS, memory_block& stack);
imported_function bool 				StringsAreEqual(const char* s1, const char* s2);

#endif
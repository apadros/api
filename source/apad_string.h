#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Conversions ******************** //

imported_function void 	ConvertStringToLowerCase(const char* s);

// All ToString() functions return a string allocated on global API memory.
imported_function char* ToString(si8 i);
imported_function char* ToString(ui8 i);
imported_function char* ToString(si16 i);
imported_function char* ToString(ui16 i);
imported_function char* ToString(si32 i);
imported_function char* ToString(ui32 i);
imported_function char* ToString(si64 i);
imported_function char* ToString(ui64 i);
												// Return limited to 2 decimal places without rounding
imported_function char* ToString(f32 f);
												// Return limited to 2 decimal places without rounding
imported_function char* ToString(f64 f);
												// @TEST
												// If string has no null-char, length must be supplied
imported_function si32 	StringToInt(const char* s, ui16 length /* Set to Null to convert up to the null-char */);

// ******************** Others ******************** //

imported_function bool IsLetter(char c);
imported_function bool IsNumber(char c);
											 // @TEST
imported_function bool IsWhitespace(char c); // Space, horizontal & vertical tabs, carriage return, newline & feed

															// Allocates string on API global memory
															// Will automatically add a null-char if target length does not contain one
imported_function 			char* AllocateString(const char* s, ui16 length /* Set to Null to copy until and including the null-char */ );
															// Allocates string on API global memory
imported_function 			char* Concatenate(ui8 count, ... /* All args must be char* */);
imported_function 			bool 	ContainsAnySubstring(const char* string, const char** substrings, ui8 subsLength);
															// Set srcLength -1 to copy entire source including the null-character
imported_function 			void 	CopyString(const char* source, si16 srcLength, const char* destination, ui16 destLength);
															// Allocates a copy on API global memory
															// If length > string length extraction will stop after the null-character
imported_function 			char* ExtractSubstring(const char* string, ui16 length /* Set to Null to extract until the null-character */);
imported_function const char* FindSubstring(const char* sub, const char* string);
															// For strings allocated on API global memory
imported_function 			void  FreeString(char* string);
															// Will return the length wihtout the null-character
imported_function 			ui16 	GetStringLength(const char* s);
imported_function 			bool  StringIsEqualToAny(const char* string, const char** strings, ui8 count);
imported_function 			bool 	StringsAreEqual(const char* s1, const char* s2);

#endif
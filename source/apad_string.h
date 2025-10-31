#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Conversions ******************** //

struct string {
	char* chars;
	ui16  length; // Without the EOS
	
	imported_function 			 string();
  imported_function 			 string(const char* s);
	imported_function 		   operator 	 char*(); // Automatic type casting to char*
	imported_function char&  operator[] (ui32 i);
	imported_function string operator+  (char c);
	imported_function void   operator+= (char c);
	imported_function string operator+  (const char* s);
	imported_function void   operator=  (const char* s);
	imported_function void   operator+= (const char* s);
	imported_function bool   operator== (string s); // For some reason can't just use the == overload below
	imported_function bool   operator== (const char* s);
};

imported_function string ToString(si8 i);
imported_function string ToString(ui8 i);
imported_function string ToString(si16 i);
imported_function string ToString(ui16 i);
imported_function string ToString(si32 i);
imported_function string ToString(ui32 i);
imported_function string ToString(si64 i);
imported_function string ToString(ui64 i);
												 // Return limited to 2 decimal places without rounding
imported_function string ToString(f32 f);
												 // Return limited to 2 decimal places without rounding
imported_function string ToString(f64 f);

imported_function si32 	 StringToInt(const char* string);

// ******************** Others ******************** //

imported_function bool IsLetter(char c);
imported_function bool IsNumber(char c);

imported_function bool 				ContainsAnySubstring(const char* string, const char** substrings, ui8 subsLength);
															// Set srcLength -1 to copy entire source including the \0
imported_function void 				CopyString(const char* source, si16 srcLength, char* destination, ui16 destLength, bool copyEOS);
															// Creates a copy.
															// Set count == -1 to extract until the end of the string.
															// Will do so automatically if count > string length.
imported_function string 			ExtractSubstring(const char* string, si8 count);
imported_function const char* FindSubstring(const char*sub, const char*string);
imported_function ui16 				GetStringLength(const char*s, bool includeEOS);
imported_function const char* PushString(const char* string, bool includeEOS, memory_block& stack);
imported_function bool 				StringsAreEqual(const char* s1, const char* s2);

#endif
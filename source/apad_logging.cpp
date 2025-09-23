#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h> // For varargs
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_logging.h"
#include "apad_string.h"

// ******************** Internal API start ******************** //

// ******************** Internal API end ******************** //

exported_function log_file OpenLogFile() {
  auto file = AllocateStack(KiB(10));
  if(ErrorIsSet() == true)
		return NullMemoryBlock;
	return file;
}

exported_function void CloseLogFile(log_file& log) {
	FreeStack(log);
}

// @TEST
// @TODO - Can the log file be set after the ... ?
// Format strings - s, si8, ui8, si16, ui16, si32, ui32, si64, ui64, f32, f64
exported_function void Log(log_file& log, const char* formatString, ...) {
  Assert(formatString != Null);
	if(ErrorIsSet() == true)
		return;
	
	va_list args;
	va_start(args, formatString);
	
  auto formatStringLength = GetStringLength(formatString, false);
  ForAll(formatStringLength) {
    char c = formatString[it];
		if(c == '%') { // Formatting char
		  it += 1;
			if(it > formatStringLength - 1) // Not enough string left for at least the 's' format char
				return;
				
			// Check for the 's' format char separately
			if(formatString[it] == 's') {
				it += 1;
				if(formatString[it] != 'i' || it == formatStringLength) { // If we're not checking for a signed integer or we have reached the end of the format string, this is a string format 
					auto s = va_arg(args, char*);
					PushString(s, false, log);
					continue; // Continue formatString scan
				}
			}
			
			// Check for remaining format substrings
			
			// 3 char-long strings
			if(it <= formatStringLength - 3) { // At least 3 chars remaining in the format string
				char testString[] = { formatString[it], formatString[it + 1], formatString[it + 2] };
								
				const char* substrings[] = { "f32", "f64" };
				ui8         substringsLength = GetArrayLength(substrings);
				bool        formatFound = false;
				ForAll(substringsLength) {
					if(StringsAreEqual(testString, substrings[it]) == true) { // Formatting substring found
					  if(it == 0) { // F32
							f32 f = (f32)va_arg(args, f32);
							auto s = ToString(f).string;
							PushString(s, false, log);
						}
						else { // F64
						  f64 f = va_arg(args, f64);
							auto s = ToString(f).string;
							PushString(s, false, log);
						}
						
						formatFound = true;
					}
				}				
				
				if(formatFound == true) {
					it += 3;
					continue; // Continue scanning formatString
				}
			}
			
			if(it <= formatStringLength - 4) { // At least 4 chars remaining in the format string
				char testString[] = { formatString[it], formatString[it + 1], formatString[it + 2], formatString[it + 3] };
				
				const char* substrings[] = { "si8", "ui8", "si16", "ui16", "si32", "ui32", "si64", "ui64" };
				ui8         substringsLength = GetArrayLength(substrings);
				bool        formatFound = false;
				ForAll(substringsLength) {
					if(StringsAreEqual(testString, substrings[it]) == true) { // Formatting substring found
					  switch(it) {
							case 0: { // SI8
								si8 i = va_arg(args, si8);
								auto s = ToString(i).string;
								PushString(s, false, log);
							  break;
							};
							case 1: { // UI8
								ui8 i = va_arg(args, ui8);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 2: { // SI16
								si16 i = va_arg(args, si16);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 3: { // UI16
								ui16 i = va_arg(args, ui16);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 4: { // SI32
								si32 i = va_arg(args, si32);
								const char* s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 5: { // UI32
								ui32 i = va_arg(args, ui32);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 6: { // SI64
								si64 i = va_arg(args, si64);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							case 7: { // UI64
								ui64 i = va_arg(args, ui64);
								auto s = ToString(i).string;
								PushString(s, false, log);
								break;
							};
							default: break;
						}
						
						formatFound = true;
						break;
					}
				}
				
				if(formatFound == true) {
					it += 4;
					continue; // Continue scanning formatString
				}
				else
					PushData(&c, sizeof(c), log); // It may be part of the string literal
			}			
		}
		else
			PushData(&c, sizeof(c), log);
  }
	va_end(args);
}
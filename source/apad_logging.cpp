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

exported_function memory_block OpenLogFile() {
  auto file = AllocateStack(KiB(10));
  if(ErrorIsSet() == true)
		return NullMemoryBlock;
	return file;
}

exported_function void CloseLogFile(memory_block& log) {
	FreeStack(log);
}

// @TODO - Can the log file be set after the ... ?
exported_function void Log(log_file& log, const char* formatString, ...) {
  Assert(formatString != Null);
	if(ErrorIsSet() == true)
		return;
	
	va_list args;
	va_start(args, formatString);
  auto length = GetStringLength(formatString, false);
  ForAll(length) {
    char c = formatString[it];
		if(c == '%') { // Formatting char
		  it += 1;
			if(it > length - 1) // Not enough string left for at least the 's' format setting
				break;
				
			// Check for the 's' format string separately
			if(formatString[it] == 's') {
				it += 1;
				if(formatString[it] != 'i' || it == length) { // If we're not checking for a signed integer or we have reached the end of the format string, this is a string format 
					auto s = va_arg(args, char*);
					PushString(s, false, log);
					continue; // Continue format string scan
				}
			}
			
			// Check for remaining format substrings
			{
				char testString[4] = { formatString[it], formatString[it + 1], formatString[it + 2], '\0' };
				it += 3;
				if(it < length) {
					testString[3] = formatString[it];
					it += 1;
				}
				
				const char* formatStrings[] = { "si8", "ui8", "si16", "ui16", "si32", "ui32", "si64", "ui64", "f32", "f64", "s" };
				ui8         formatStringsLength = GetArrayLength(formatStrings);
				bool        formatFound = false;
				ForAll(formatStringsLength) {
					if(StringsAreEqual(testString, formatStrings[it]) == true) { // Formatting string found
						formatFound = true;
						
						if(it == 0) { // SI8
						}
						else if(it == 1) { // UI8
						}
						else if(it == 2) { // SI16
						}
						else if(it == 3) { // UI16
						}
						else if(it == 4) { // SI32
						}
						else if(it == 5) { // UI32
						}
						else if(it == 6) { // SI64
						}
						else if(it == 7) { // UI64
						}
						else if(it == 8) { // F32
							f32 f = (f32)va_arg(args, f64); // Has to be cast to f64 since that is the default
							auto s = F32ToString(f).string;
							PushString(s, false, log);
						}
						else if(it == 9) { // F64
						}
					}
				}
			}
			
			// @TODO - If the format string has no matches, error
			
			
			c = string[it];
			if(c == '\0')
				break;
			
			// @TODO - Add all types
			if(c == 'si32') { // SI32 // @TODO - Replace with string function
				si64 i = va_arg(args, si32);
				const char* s = I64ToString(i).string;
				PushString(s, false, log);
			}
			
		}
		else
			PushData(&c, sizeof(c), log);
  }
	va_end(args);
}
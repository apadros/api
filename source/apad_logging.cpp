#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h> // For varargs
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
exported_function void Log(log_file& log, const char* string, ...) {
  Assert(string != Null);
	if(ErrorIsSet() == true)
		return;
	
	va_list args;
	va_start(args, string);
  auto length = GetStringLength(string, false);
  ForAll(length) {
    char c = string[it];
		if(c == '%' && it < length - 1) { // Formatting string
		  it += 1;
			if(length - it < 3) // Not enough string left for correct format setting
				break;
			
			const char* formatStrings[] = { "si8", "ui8", "si16", "ui16", "si32", "ui32", "si64", "ui64", "f32", "f64" };
			if(length - it == 3) { // 3 chars left
			  const char* formatStrings[] = { "si8", "ui8", "f32", "f64" };
			
			}
			else if(length - it == 4) { // 4 chars left
			  
			}
			else { // More than 4 chars left in string
			  char temp = string[it + 4];
				string[it + 4] = '\0';
				
				// @TODO - Get array length function
				bool found = ContainsAnySubstring(string, formateStrings, const char** substrings, ui8 length);

				
				string[it + 4] = temp;
			}
			
			c = string[it];
			if(c == '\0')
				break;
			
			if(c == 'si32') { // SI32
				si64 i = va_arg(args, si32);
				const char* s = I64ToString(i).string;
				PushString(s, false, log);
			}
			else if(c == 'f') { // Floating point
				f32 f = (f32)va_arg(args, f64); // Has to be cast to f64 since that is the default
				auto s = F32ToString(f).string;
				PushString(s, false, log);
			}
			else if(c == 's') { // String
				auto s = va_arg(args, char*);
				PushString(s, false, log);
			}
		}
		else
			PushData(&c, sizeof(c), log);
  }
	va_end(args);
}
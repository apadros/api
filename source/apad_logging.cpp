#include <stdarg.h> // For varargs
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_string.h"

// To avoid compiler silliness when compilling the dll
#ifdef imported_function
#undef imported_function
#define imported_function exported_function
#endif
#include "apad_logging.h"

// ******************** Internal API start ******************** //

// ******************** Internal API end ******************** //

exported_function memory_block OpenLogFile() {
  auto file = AllocateStack(KiB(10));
  if(ErrorIsSet() == true)
		return NullMemoryBlock;
	return file;
  // Log("\n");
}

exported_function void CloseLogFile(memory_block& log) {
	FreeStack(log);
}

exported_function void Log(log_file& log, const char* string, va_list args) {
  Assert(string != Null);
	if(ErrorIsSet() == true)
		return;
	
  auto length = GetStringLength(string, false);
  ForAll(length) {
    char c = string[it];
		if(c == '%' && it < length - 1) { // Formatting string
			it += 1;
			c = string[it];
			if(c == 'i') { // Integer
				auto i = va_arg(args, si32);
				// @TODO - IntToString()
				// @TODO - PushString()
			}
			else if(c == 'f') { // Floating point
				auto f = (f32)va_arg(args, f64); // Has to be cast to f64 since that is the default
				// @TODO - F32ToString()
				// @TODO - PushString()
			}
			else if(c == 's') { // String
				auto s = va_arg(args, char*);
				// @TODO - PushString()
			}
		}
		else
			PushData(&c, sizeof(c), log);
  }
}
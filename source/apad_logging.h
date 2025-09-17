#ifndef APAD_LOGGING_H
#define APAD_LOGGING_H

// #include <stdarg.h> // For va_list
#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

typedef memory_block log_file;

#define NullLogFile NullMemoryBlock

imported_function log_file OpenLogFile();
													 // %i for 64 bit signed integers
													 // %f for floats
													 // %s for strings
imported_function void 		 Log(log_file& file, const char* formatString, ... );
imported_function void 		 CloseLogFile(log_file& log);

#endif
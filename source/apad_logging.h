#ifndef APAD_LOGGING_H
#define APAD_LOGGING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

typedef memory_block log_file;

imported_function log_file OpenLogFile();
imported_function void 		 CloseLogFile(log_file& log);

#endif
#ifndef APAD_LOGGING_H
#define APAD_LOGGING_H

#include "apad_intrinsics.h"
#include "apad_memory.h"

imported_function memory_block OpenLogFile();
imported_function void 				 CloseLogFile(memory_block& log);

#endif
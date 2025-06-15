#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Internal API start ******************** //


// program_local memory_block logFile = NullMemoryBlock;

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
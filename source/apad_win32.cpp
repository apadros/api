#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include "apad_base_types.h"
#include "apad_debug_error.h"
#include "apad_intrinsics.h"

// ******************** Local API start ******************** //



// ******************** Local API end ******************** //

exported_function void* AllocateWin32Memory(ui32 size) {
  void* errorRet = Null;
	
  Assert(size > 0);
	if(ErrorIsSet() == true)
		return errorRet;
  
  void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  Assert(mem != NULL);
	if(ErrorIsSet() == true)
		return errorRet;
	
  return mem;
}

exported_function void FreeWin32Memory(void* mem) {
	Assert(mem != Null);
	if(ErrorIsSet() == true)
		return;
	
  auto ret = VirtualFree(mem, 0, MEM_RELEASE);
	Assert(ret != 0);
}

program_local int main() {
	
	return 0;
}
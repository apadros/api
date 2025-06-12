// #define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include "apad_base_types.h"
#include "apad_debug_error.h"

void* AllocateWin32Memory(ui32 size) {
  Assert(size > 0);
  
  void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  Assert(mem != NULL);
  
  return mem;
}

void FreeWin32Memory(void* mem) {
  auto ret = VirtualFree(mem, 0, MEM_RELEASE);
	Assert(ret != 0);
}

int main() {
	void* mem = AllocateWin32Memory(1024);
	
	FreeWin32Memory(mem);
	
	return 0;
}
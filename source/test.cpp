#include "apad_base_types.h"
#include "apad_debug_error.h"
#include "apad_win32.h"
int main() {
	void* mem = AllocateWin32Memory(0);
	
	bool error = ErrorIsSet();
	const char* errorString = GetError();
	
	FreeWin32Memory(mem);
	
	error = ErrorIsSet();
	
	return 0;
}
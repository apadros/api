#ifndef APAD_ERROR_INTERNAL_H
#define APAD_ERROR_INTERNAL_H

#include "apad_error.h"
#include "apad_intrinsics.h"

#ifdef APAD_DEBUGGER_ASSERTIONS

#define FunctionStart(_return) ;

#define AssertInternal(_condition) \
	Assert(_condition)

#else

// Stack unwinding mechanics
#include <setjmp.h> // For jmp_buf, setjmp() & longjmp()
#include "apad_base_types.h"
program_external jmp_buf JumpBuffer;
program_external si8 		 ReturnFromAPI;

// This MUST be called at the start of every internal function which calls another API function or uses internal assertions before any such calls
#define FunctionStart(_return /* Must contain a ; at least */) { \
	if(ReturnFromAPI == -1) /* Unset */ \
		ReturnFromAPI = setjmp(JumpBuffer); /* setjmp() will return 0 when called. When longjmp() is called, stack will be rewinded to this point and setjmp() will return 1 */ \
	if(ReturnFromAPI == 1) \
			return _return; /* Exit API */ \
}

#include <stdio.h> // For sprintf
#include "apad_file.h"
#define AssertInternal(_condition) { \
	if(!(_condition)) { \
	 	char buffer[256] = {}; \
		sprintf(buffer, "[APAD_API] Internal assertion failed. \
	  								  \n[Condition] %s \
	  									\n[File]      %s \
	  									\n[Line]      %lu", #_condition, GetFileNameAndExtension(__FILE__), __LINE__); \
	 	SetGlobalError((const char*)buffer); \
		program_external bool DisplayInternalAssertions; \
		if(DisplayInternalAssertions == true) \
			DisplayGlobalError(); \
		longjmp(JumpBuffer, 1); /* Unwind the call stack to the initial setjmp() call */ \
	} \
}
	
#endif

#endif
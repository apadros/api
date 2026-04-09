#ifndef APAD_ERROR
#define APAD_ERROR

#include "apad_intrinsics.h"

// ******************** Generic ******************** //

dll_import void ExitProgram(bool error); // This will not work within DLL functions

// ******************** Error checking, setting and displaying ******************** //

// Use these to create an error message which is then logged by outside code (e.g. error within a function, need it available when returning)
dll_import bool 			 IsExitIfGlobalErrorSet();
											 // False by default
dll_import void 			 SetExitIfGlobalError(bool b);

dll_import void 			 ClearGlobalError();
dll_import bool 			 GlobalErrorIsSet();
dll_import const char* GetGlobalError();
dll_import void 			 SetGlobalError(const char* string);
dll_import void 			 DisplayGlobalError();
											 // Does not set the global error
dll_import void 			 DisplayError(const char* string);

dll_import void 			 SetDisplayInternalAssertions(bool b); // True by default

/******************** Assertions ******************** 

#define APAD_DEBUGGER_ASSERTIONS for use in a debugger, do NOT otherwise as they will have no effect.
If the macro is not defined, assertions set the global error and record the failed condition in a global string.
Call SetExitIfError(true) to have assertions stop program execution, otherwise will continue by default to allow client code to handle as seen fit.
Assertions will be printed in command line programs and displayed in a message box in GUI programs.

*****************************************************/

// Assert()
#include <intrin.h> // For __debugbreak()
#ifdef APAD_DEBUGGER_ASSERTIONS

// Will break into the debugger in debug mode and stop and exit program execution in release mode
#define Assert(_condition) { \
  if(!(_condition)) \
		__debugbreak(); \
}

#else

// If IsExitIfErrorSet() == true, will exit program execution. Otherwise, need to 
// manually check afterwards for errors and manually decide execution from there.
// IsExitIfErrorSet() == false by default.
#include <stdio.h> // For sprintf
#define Assert(_condition) { \
	dll_import program_external bool AssertionsEnabled; \
	if(AssertionsEnabled == true) { \
		AssertionsEnabled = false; \
    ClearGlobalError(); \
    if(!(_condition)) { \
	  	char buffer[256] = {}; \
			program_external const char* GetFileNameAndExtension(const char*); \
	  	sprintf(buffer, "Assertion failed \
	  									 \n[Condition] %s \
	  									 \n[File]      %s \
	  									 \n[Line]      %lu", #_condition, GetFileNameAndExtension(__FILE__), __LINE__); \
	  	SetGlobalError((const char*)buffer); \
	  	DisplayGlobalError(); \
			ClearGlobalError(); /* To avoid triggering checks within Win32PrintStackBackTrace() */ \
			program_external void Win32PrintStackBackTrace(); \
			Win32PrintStackBackTrace(); \
	  	AssertionsEnabled = true; \
	  	if(IsExitIfGlobalErrorSet() == true) \
	  	  ExitProgram(true); \
	  } \
		AssertionsEnabled = true; \
	} \
}

#endif

// AssertRet()
#define AssertRet(_condition) { \
	Assert(_condition); \
	program_external bool AssertionsEnabled; \
	if(AssertionsEnabled == true && GlobalErrorIsSet() == true) \
	  return; \
}

// AssertRetType()
#define AssertRetType(_condition, _retValue) { \
	Assert(_condition); \
	program_external bool AssertionsEnabled; \
	if(AssertionsEnabled == true && GlobalErrorIsSet() == true) \
	  return (_retValue); \
}

// InvalidCodePath
#define InvalidCodePath Assert(false)

#endif
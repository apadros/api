#ifndef APAD_ERROR
#define APAD_ERROR

#include "apad_intrinsics.h"

// ******************** Generic ******************** //

imported_function void ExitProgram(bool error); // This will not work within DLL functions

// ******************** Error checking and setting ******************** //

// Use these to create an error message which is then logged by outside code (e.g. error within a function, need it available when returning)
imported_function bool 				IsExitIfErrorSet();
imported_function void 				SetExitIfError(bool b); // False by default

imported_function void 				ClearError();
imported_function bool 				ErrorIsSet();
imported_function const char* GetError();
imported_function void 				SetError(const char* string);

// ******************** Assertions ******************** //

// Assert()
#include <intrin.h> // For __debugbreak()
#ifdef APAD_DEBUG

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
  ClearError(); \
  if(!(_condition)) { \
	  char buffer[256] = {}; \
		sprintf(buffer, "Assertion failed \
										 \nCondition - %s, \
										 \nFile      - %s, \
										 \nLine      - %lu", #_condition, __FILE__, __LINE__); \
		SetError((const char*)buffer); \
		if(IsExitIfErrorSet() == true) \
		  ExitProgram(true); \
	} \
}

#endif

// AssertRet()
#ifdef APAD_DEBUG

#define AssertRet(_condition) \
	Assert(_condition)

#else
	
#define AssertRet(_condition) { \
	Assert(_condition); \
	if(ErrorIsSet() == true) \
	  return; \
}

#endif

// AssertRetType()
#ifdef APAD_DEBUG

#define AssertRetType(_condition, _retValue) \
	Assert(_condition)

#else
	
#define AssertRetType(_condition, _retValue) { \
	Assert(_condition); \
	if(ErrorIsSet() == true) \
	  return (_retValue); \
}

#endif

#endif
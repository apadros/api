#ifndef APAD_ERROR
#define APAD_ERROR

#include "apad_intrinsics.h"

// ******************** Generic ******************** //

imported_function void ExitProgram(bool error); // This will not work within DLL functions

// ******************** Error checking and setting ******************** //

// Use these to create an error message which is then logged by outside code (e.g. error within a function, need it available when returning)
imported_function bool 				IsExitIfErrorSet();
imported_function void 				SetExitIfError(bool b);

imported_function void 				ClearError();
imported_function bool 				ErrorIsSet();
imported_function const char* GetError();
imported_function void 				SetError(const char* string);

// ******************** Assertions ******************** //

#include <intrin.h> // For __debugbreak()
#ifdef APAD_DEBUG

#define Assert(_condition) { \
  if(!(_condition)) \
		__debugbreak(); /* This will stop and exit program execution in release mode */ \
}

#define AssertRet(_condition, _retValue) \
	Assert(_condition)

#else
	
#define Assert(_condition) { \
  ClearError(); \
  if(!(_condition)) { \
		SetError("ERROR - " ## #_condition ## ", file " ## __FILE__); \
		if(IsExitIfErrorSet() == true) \
		  ExitProgram(true); \
	} \
}
	
#define AssertRet(_condition, _retValue) { \
	Assert(_condition); \
	if(ErrorIsSet() == true) \
	  return (_retValue); \
}

#endif

#endif
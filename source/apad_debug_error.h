#ifndef APAD_DEBUG_ERROR
#define APAD_DEBUG_ERROR

#include "apad_intrinsics.h"

// ******************** Generic ******************** //

imported_function void ExitProgram(bool error);

// ******************** Error checking and setting ******************** //

// Use these to create an error message which is then logged by outside code (e.g. error within a function, need it available when returning)
imported_function void 				SetExitIfError(bool b);
imported_function void 				ClearError();
imported_function bool 				ErrorIsSet();
imported_function const char* GetError();
imported_function void 				SetError(const char* string);

// ******************** Assertions ******************** //

#include "apad_base_types.h"
#ifdef BUCKET_DEBUG

#define Assert(_condition) { \
  if(!(_condition)) { \
		ui64* ptr = Null; \
    ui64 _u = *ptr; \
	} \
}

#else
	
#define Assert(_condition) \
  if(!(_condition)) { \
		SetError("ERROR - " ## #_condition ## ", file " ## __FILE__); \
		program_external bool exitIfError;\
		if(exitIfError == true) \
		  ExitProgram(true); \
	}

#endif

#endif
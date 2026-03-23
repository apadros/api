#ifndef APAD_ERROR_INTERNAL_H
#define APAD_ERROR_INTERNAL_H

#include "apad_error.h"
#include "apad_intrinsics.h"

#define AssertionErrorValueZero 			 "value is 0"
#define AssertionErrorNullString 			 "string is Null"
#define AssertionErrorStringLengthZero "string length is 0"

#ifdef APAD_DEBUGGER_ASSERTIONS

#define AssertInternal(_condition, _functionID, _errorString) \
	Assert(_condition)

#else
	
#include <stdio.h> // For sprintf
#define AssertInternal(_condition) { \
	if(!(_condition)) { \
	 	char buffer[256] = {}; \
		sprintf(buffer, "[APAD_API] Internal assertion failed. \
	  								  \n[Condition] %s \
	  									\n[File]      %s \
	  									\n[Line]      %lu", #_condition, GetFileNameAndExtension(__FILE__), __LINE__); \
	 	SetGlobalError((const char*)buffer); \
	} \
}
	
#endif


#define AssertRetInternal(_condition, _errorString, _functionIDString) { \
	AssertInternal(_condition, _errorString, _functionIDString); \
	if(!(_condition)) \
		return; \
}

#define AssertRetTypeInternal(_condition, _errorString, _functionIDString, _retValue) { \
	AssertInteral(_condition, _errorString, _functionIDString); \
	if(!(_condition)) \
		return (_retValue); \
}


#endif
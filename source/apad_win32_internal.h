#ifndef APAD_WIN32_INTERNAL_H
#define APAD_WIN32_INTERNAL_H

#include "apad_error_internal.h"

#ifdef APAD_DEBUGGER_ASSERTIONS

#define AssertInternalWin32(_condition) \
	Assert(_condition)

#else

// Can be used to check multiple times before next Win32 call since GetLastError() does NOT clear the error
#define AssertInternalWin32(_condition) { \
	BeginInternalAssertion(_condition) \
	sprintf(buffer, "%s\n  [Last Windows error] %u", (const char*)buffer, GetLastError()); \
	EndInternalAssertion() \
}

#endif

#endif
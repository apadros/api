#ifndef APAD_WIN32_INTERNAL_H
#define APAD_WIN32_INTERNAL_H

#include "apad_error_internal.h"

#define AssertInternalWin32(_condition) { \
	BeginInternalAssertion(_condition) \
	sprintf(buffer, "%s\n  [Last Windows error] %u", (const char*)buffer, GetLastError()); \
	EndInternalAssertion() \
}


#endif
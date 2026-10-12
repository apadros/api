#ifndef APAD_STRING_INTERNAL_H
#define APAD_STRING_INTERNAL_H

#include "apad_error_internal.h"
#include "apad_string.h"

// @WIP
#define AssertStringInternal(_string, _length) { \
	AssertInternal(string != Null); \
	if(_length == Null) \
		_length = GetLength(_string); \
	AssertInternal(_length != Null); \
}

#endif
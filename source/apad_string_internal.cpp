#include "apad_error_internal.h"
#include "apad_string.h"

void CheckStringInternal(char*& string, ui16& length) {
	AssertInternal(string != Null);
	if(length == Null)
		length = GetLength(string);
	AssertInternal(length != Null);
}
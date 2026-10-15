#ifndef APAD_STRING_INTERNAL_H
#define APAD_STRING_INTERNAL_H

#include "apad_base_types.h"
void CheckStringInternal(char*& string, ui16& length); // If length == 0, will assume null-terminated string and will set length with GetLength()

#endif
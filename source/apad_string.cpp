#include <stdio.h>
#include <stdlib.h> // For atexit()
#include <string.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_memory.h"
#include "apad_string.h"
#include "apad_string_internal.h"

// ******************** Internal API start ******************** //

program_local char* PushNullChar(memory_stack& stack) {
	FunctionStart(Null);
	
	void* ret = Push((void*)"\0", 1, stack);
	
	FunctionEnd();
	return (char*)ret;
}

// ******************** Internal API end ******************** //

dll_export char* Push(char* string, ui16 length, bool addEOS, memory_stack& stack) {
  FunctionStart(Null);
	
	if(string == Null) {
		AssertInternal(addEOS == true);
		auto ret = PushNullChar(stack);
		FunctionEnd();
		return ret;
	}
	
	if(length == Null)
		length = GetLength(string);
	
	void* ret = Push((void*)string, length, stack);
	
	if(addEOS == true)
		PushNullChar(stack);
	
	FunctionEnd();
	return (char*)ret;
}

#include <ctype.h>
dll_export bool IsWhitespace(char c) {
	return isspace(c) != 0;
}

dll_export char* ConvertStringToLowerCase(char* s, ui16 length) {
	FunctionStart(Null);
	CheckStringInternal(s, length);
	
	char* ret = AllocateString(s, length);
	
	ForAll(length) {
    if(ret[it] >= 'A' && ret[it] <= 'Z')
			ret[it] += 'a' - 'A';
	}
	
	FunctionEnd();
	return ret;
}

#include <stdarg.h>
// Unfortunately variadic arguments cannot intrinsically infer the number of parameters passed to them
dll_export char* Concatenate(ui8 count, ...) {
	FunctionStart(Null);
	AssertInternal(count >= 2);
	
	va_list list;
	va_start(list, count);
	
	auto stack = AllocateStack(Null);
	
	ForAll(count) {
		char* string = va_arg(list, char*);
		if(string != Null) // Just to avoid having to check for Null when concatenating several strings
			Push(string, Null, false, stack);
	}
	
	PushNullChar(stack);
	
	va_end(list);
	
	FunctionEnd();
	return (char*)stack.memory.memory;
}

dll_export char* AllocateString(char* s, ui16 length) {
	FunctionStart(Null);
	CheckStringInternal(s, length);

	auto stack = AllocateStack(length + 1);
	Push(s, length, true, stack);
	
	FunctionEnd();
	return (char*)GetMemory(stack);
}

dll_export ui16 GetLength(char* s) {
  FunctionStart(Null);
	AssertInternal(s != Null);
  
				bool found = false;
				ui16 length = 0;
	const ui16 max = 300; // On Windows MAX_PATH is 255 - 260 without extended paths
	for(; length < max; length++) {
		if(s[length] == '\0') {
			found = true;
			break;
		}
	}
	
	FunctionEnd();
  
	return found == true ? length : Null;
};

dll_export char* ToString(si8 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%hhi", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(ui8 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%hhu", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(si16 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%hi", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(ui16 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%hu", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(si32 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%li", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(ui32 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%lu", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(si64 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%lli", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(ui64 i) {
  FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%llu", i);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(f32 f) {
	FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2f", f);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export char* ToString(f64 f) {
	FunctionStart(Null);
	
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2Lf", f);
	auto ret = AllocateString((char*)buffer, Null);
	
	FunctionEnd();
	return ret;
}

dll_export bool AreEqual(char* s1, ui16 s1Length, char* s2, ui16 s2Length) {
  FunctionStart(false);
	
	if(s1 == Null || s1 == Null) {
		FunctionEnd();
		return false;
	}
	
	CheckStringInternal(s1, s1Length);
	CheckStringInternal(s2, s2Length);
		
	if(s1Length != s2Length) {
		FunctionEnd();
		return false;
	}
	
	ForAll(s1Length) {
		if(s1[it] != s2[it]) {
			FunctionEnd();
			return false;
		}
	}
	
	FunctionEnd();
	return true;
}

dll_export char* FindSubstring(char* sub, ui16 subLength, char* string, ui16 stringLength) {
  FunctionStart(Null);
	CheckStringInternal(sub, subLength);
	CheckStringInternal(string, stringLength);
	
	ForAll(stringLength) {
		if(AreEqual(sub, subLength, string + it, subLength) == true) {
			FunctionEnd();
			return string + it;
		}
	}
  
	FunctionEnd();
	return Null;
}

dll_export bool ContainsAnySubstring(char* string, ui16 length, char** substrings, ui8 subsCount) {
  FunctionStart(false);
	CheckStringInternal(string, length);
	
	ForAll(subsCount) {
    auto* sub = substrings[it];
		AssertInternal(sub != Null);
		if(FindSubstring(sub, Null, string, length) != Null) {
			FunctionEnd();
			return true;
		}
	}
	
	FunctionEnd();
  return false;
}

dll_export void Copy(char* source, ui16 srcLength, char* destination, ui16 destLength) {
	FunctionStart(;);
	
	CheckStringInternal(source, srcLength);
	CheckStringInternal(destination, destLength);
	AssertInternal(srcLength >= destLength);
	
	Copy((void*)source, srcLength, (void*)destination);
	
	FunctionEnd();
}

dll_export bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

dll_export bool IsWord(char* string, ui16 length) {
	FunctionStart(false);
	CheckStringInternal(string, length);
	
	ForAll(length) {
		if(IsLetter(string[it]) == false)
			return false;
	}
	
	FunctionEnd();
	return true;
}

dll_export bool IsNumber(char c) {
	return c >= '0' && c <= '9';
}

dll_export bool IsNumber(char* string, ui16 length) {
	FunctionStart(false);
	CheckStringInternal(string, length);
	
	ForAll(length) {
		if(IsNumber(string[it]) == false)
			return false;
	}
		
	FunctionEnd();
	return true;
}

#include <stdlib.h>
dll_export si32 StringToInt(char* string, ui16 length) {
  FunctionStart(0);
	CheckStringInternal(string, length);
	
	// Want to convert only part of the string or string is an array without a null char
	auto copy = AllocateString(string, length);
	auto ret = atoi(copy);
	Free(copy);
	
	FunctionEnd();
	return ret;
}

dll_export char* ExtractSubstring(char* s, ui16 length) {
	FunctionStart(Null);
	
	auto ret = AllocateString(s, length);
	
	FunctionEnd();
	return ret;
}

dll_export bool StringIsEqualToAny(char* string, ui16 length, char** strings, ui8 count) {
	FunctionStart(false);
	CheckStringInternal(string, length);
	AssertInternal(strings != Null);
	AssertInternal(count > 0);
	
	ForAll(count) {
		auto s = strings[it];
		AssertInternal(s != Null);
		if(AreEqual(string, length, s, Null) == true) {
			FunctionEnd();
			return true;
		}
	}
	
	FunctionEnd();
	return false;
}
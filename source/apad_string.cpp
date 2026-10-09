#include <stdio.h>
#include <stdlib.h> // For atexit()
#include <string.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_memory.h"
#include "apad_string.h"

// ******************** Internal API start ******************** //

program_local char* PushNullChar(memory_stack& stack) {
	FunctionStart(;);
	
	void* ret = Push((void*)"\0", 1, stack);
	
	FunctionEnd();
	return (char*)ret;
}

// Also used in log.cpp
dll_export char* Push(const char* string, ui16 length, bool addEOS, memory_stack& stack) {
  FunctionStart(Null);
	AssertInternal(string != Null || addEOS == true);
	
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

// ******************** Internal API end ******************** //

#include <ctype.h>
dll_export bool IsWhitespace(char c) {
	return isspace(c) != 0;
}

dll_export char* ConvertStringToLowerCase(const char* s, ui16 length) {
	FunctionStart(Null);
	AssertInternal(s != Null);
	
	char* ret = AllocateString(s, length);
	
	if(length == Null)
		length = GetLength(s);
	
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

dll_export char* AllocateString(const char* s, ui16 length) {
	FunctionStart(Null);
	AssertInternal(s != Null);

	// Set the real length without the EOS char
	if(length == Null)
		length = GetLength(s);
	else if(s[length - 1] == '\0')
		length -= 1;
	
	auto stack = AllocateStack(length + 1);
	Push((void*)s, length, false, stack);
	PushNullChar(stack);
	
	FunctionEnd();
	return (char*)GetMemory(stack);
}

dll_export ui16 GetLength(const char* s) {
  FunctionStart(0);
	AssertInternal(s != Null);
  
	auto length = strlen(s);
	AssertInternal(length <= UI16Max);
  
	FunctionEnd();
  return length;
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

dll_export bool AreEqual(const char* s1, ui16 s1Length, const char* s2, ui16 s2Length) {
  FunctionStart(false);
	
	AssertInternal(s1 != Null);
	AssertInternal(s2 != Null);
	
	if(s1Length == Null)
		s1Length = GetLength(s1);
	if(s2Length == Null)
		s2Length = GetLength(s2);
	
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

dll_export const char* FindSubstring(const char* sub, ui16 subLength, const char* string, ui16 stringLength) {
  FunctionStart(Null);
	
	AssertInternal(string != Null);
  AssertInternal(sub != Null);
	
	if(subLength == Null)
		subLength = GetString(sub);
	if(stringLength == Null)
		stringLength = GetLength(string);
	
	ForAll(stringLength) {
		if(AreEqual(sub, subLength, string + it, subLength) == true) {
			FunctionEnd();
			return string + it;
		}
	}
  
	FunctionEnd();
	return Null;
}

dll_export bool ContainsAnySubstring(const char* string, ui16 length, const char** substrings, ui8 subCount) {
  FunctionStart(false);
	
	if(length == Null)
		length = GetLength(string);
	
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
	
	AssertInternal(source != Null);
	AssertInternal(destination != Null);
	
	if(srcLength == Null)
		srcLength = GetLength(source);
	if(destLength == Null)
		destLength = GetLength(destination);
	
	AssertInternal(srcLength >= destLength);
	
	Copy((void*)source, srcLength, (void*)destination);
	
	FunctionEnd();
}

dll_export bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

dll_export bool IsWord(char* string, ui16 length) {
	FunctionStart(false);
	AssertInternal(string != Null);
	
	if(length == Null)
		length = GetLength(string);
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
	AssertInternal(string != Null);
	
	if(length == Null)
		length = GetLength(string);
	ForAll(length) {
		if(IsNumber(string[it]) == false)
			return false;
	}
		
	FunctionEnd();
	return true;
}

#include <stdlib.h>
dll_export si32 StringToInt(const char* string, ui16 length) {
  FunctionStart(0);
	AssertInternal(string != Null);
	
	si32 ret = Null;
	
	if(length == Null)
		ret = atoi(string);
	else { // Want to convert only part of the string or string is an array without a null char
		auto copy = AllocateString(string, length);
		ret = atoi(copy);
		Free(copy);
	}
	
	FunctionEnd();
	return ret;
}

dll_export char* ExtractSubstring(const char* s, ui16 length) {
	FunctionStart(Null);
	AssertInternal(s != Null);
	
	auto sLength = GetLength(s);
	AssertInternal(sLength > 0);
	
	ui8  copyLength = 0;
	if(length == Null || length > sLength)
		copyLength = sLength;
	else
		copyLength = length;
	
	auto stack = AllocateStack(copyLength + 1);
	// Use the memory versions since the string versions will push the entire string
	// instead of just a section if that is what's wanted.
	void* mem = Push((void*)s, copyLength, stack); 
	PushNullChar(stack);
	
	FunctionEnd();
	return (char*)stack.memory.memory;
}

dll_export bool StringIsEqualToAny(const char* string, ui16 length, const char** strings, ui8 count) {
	FunctionStart(false);
	AssertInternal(string != Null);
	AssertInternal(strings != Null);
	AssertInternal(count > 0);
	
	ForAll(count) {
		auto s = strings[it];
		AssertInternal(s != Null);
		if(AreEqual(string, length, s) == true) {
			FunctionEnd();
			return true;
		}
	}
	
	FunctionEnd();
	return false;
}
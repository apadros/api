#include <stdio.h>
#include <stdlib.h> // For atexit()
#include <string.h>
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_memory.h"
#include "apad_string.h"

// ******************** Local API start ******************** //

const ui16 MaxStringLength = UI16Max;

// @TODO - Replace stack functionality with pool allocation ?
// Array of memory_blocks containing dynamically-allocated memory pointers allocated on the heap.
memory_stack stringTable; 

program_local void ExitStringAPI() {
	if(IsValid(stringTable) == true) {
		// Free all memory blocks contained within the table
		ui32  blocks = stringTable.size / sizeof(memory_block);
		auto* table = (memory_block*)stringTable.memory;
		ForAll(blocks) {
			auto* block = table + it;
			FreeMemory(*block);
		}
		
		FreeStack(stringTable);
	}
}

program_local void AddToStringTable(memory_block& block) {
  if(IsValid(stringTable) == false) { // Init table
	  stringTable = AllocateStack(KiB(1));
		atexit(ExitStringAPI); // @TODO - Returns 0 for no error. What to do if it doesn't?
	}
	
	PushInstance(block, stringTable);
}

program_local void PushNullChar(memory_stack& stack) {
	Push("\0", 1, stack);
}

// ******************** Local API end ******************** //

exported_function bool IsWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n';
}

exported_function void ConvertStringToLowerCase(char* s) {
	AssertRet(s != Null);
	
	auto length = GetStringLength(s);
	ForAll(length) {
    if(s[it] >= 'A' && s[it] <= 'Z')
			s[it] += 'a' - 'A';
	}
}

#if 0 // @CLEANUP
exported_function const char* Concatenate(const char* s1, const char* s2) {
  AssertRetType(s1 != Null, Null);
  AssertRetType(s2 != Null, Null);
	
	auto length1 = GetStringLength(s1, false);
	auto length2 = GetStringLength(s2, true);
	
	auto block = AllocateMemory(length1 + length2);
	
	void* mem = block.memory;
	CopyMemory((void*)s1, length1, mem);
	CopyMemory((void*)s2, length2, (ui8*)mem + length1);
	
	AddToStringTable(block);
		
	string ret;
	ret.chars = (char*)mem;
	ret.length = length1 + length2;
	return ret;
}
#endif

#include <stdarg.h>
// Unfortunately variadic arguments cannot intrinsically infer the number of parameters passed to them
exported_function const char* Concatenate(ui8 count, ...) {
	AssertRetType(count >= 2, Null);
	
	va_list list;
	va_start(list, count);
	
	auto stack = AllocateStack(Null);
	
	ForAll(count) {
		const char* string = va_arg(list, const char*);
		Push((void*)string, GetStringLength(string), stack);
	}
	
	Push("\0", 1, stack);
	
	va_end(list);
	
	AddToStringTable(stack);
	
	return (const char*)stack.memory;
}

exported_function const char* AllocateString(const char* s, ui16 length) {
	AssertRetType(s != Null, Null);
	
	auto sLength = GetStringLength(s);
	
	ui16 copyLength = 0;
	if(length == Null)
		copyLength = sLength;
	else
		copyLength = Min(length, sLength);
	
	auto stack = AllocateStack(copyLength + 1);
	Push((void*)s, copyLength, stack);
	PushNullChar(stack);
	
	AddToStringTable(stack);
	
	return (const char*)stack.memory;
}

exported_function ui16 GetStringLength(const char* s) {
  AssertRetType(s != Null, Null);
  
	auto length = strlen(s);
	AssertRetType(length <= UI16Max, Null);
  
  return length;
};

exported_function const char* ToString(si8 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hhi", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(ui8 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hhu", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(si16 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hi", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(ui16 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hu", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(si32 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%li", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(ui32 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%lu", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(si64 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%lli", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(ui64 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%llu", i);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(f32 f) {
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2f", f);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* ToString(f64 f) {
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2Lf", f);
	return AllocateString((const char*)buffer, Null);
}

exported_function const char* PushString(const char* string, bool addEOS, memory_block& stack) {
  auto length = GetStringLength(string);
	if(ErrorIsSet() == true)
		return Null;
	void* mem = Push((void*)string, length, stack);
	if(addEOS == true)
		Push("\0", 1, stack);
	return (const char*)mem;
}

exported_function bool StringsAreEqual(const char* s1, const char* s2) {
  AssertRetType(s1 != Null, false);
	AssertRetType(s2 != Null, false);
	
	return strcmp(s1, s2) == 0;
}

exported_function const char* FindSubstring(const char*sub, const char*string) {
  AssertRetType(string != Null, Null);
  AssertRetType(sub != Null, Null);
  return strstr(string, sub);
}

exported_function bool ContainsAnySubstring(const char* string, const char** substrings, ui8 length) {
  ForAll(length) {
    auto* sub = substrings[it];
		AssertRetType(sub != Null, false);
		if(FindSubstring(sub, string) != Null)
			return true;
	}
  return false;
}

exported_function void CopyString(const char* source, si16 srcLength, char* destination, ui16 destLength) {
	AssertRet(source != Null);
	AssertRet(srcLength > 0 || srcLength == -1);
	AssertRet(destination != Null);
	AssertRet(destLength > 0);
	
	auto copyLength = srcLength == -1 ? (GetStringLength(source) + 1) : srcLength;
	AssertRet(copyLength <= destLength);
	
	CopyMemory((void*)source, copyLength, (void*)destination);
}

exported_function bool IsLetter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

exported_function bool IsNumber(char c) {
	return c >= '0' && c <= '9';
}

#include <stdlib.h>
exported_function si32 StringToInt(const char* string) {
  AssertRetType(string != Null, Null);
  return atoi(string);
}

exported_function const char* ExtractSubstring(const char* s, ui8 length) {
	AssertRetType(s != Null, Null);
	
	auto sLength = GetStringLength(s);
	AssertRetType(sLength > 0, Null);
	
	ui8  copyLength = 0;
	if(length == Null || length > sLength)
		copyLength = sLength;
	else
		copyLength = length;
	
	auto stack = AllocateStack(copyLength + 1);
	Push((void*)s, copyLength, stack);
	Push("\0", 1, stack);
	
	AddToStringTable(stack);
	
	return (const char*)stack.memory;
}
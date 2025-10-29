#include <stdio.h>
#include <stdlib.h> // For atexit()
#include <string.h>
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_memory.h"
#include "apad_string.h"
#include "apad_win32.h"

// ******************** Local API start ******************** //

const ui16 MaxStringLength = UI16Max;

// @TODO - Replace stack functionality with pool allocation ?
memory_block stringTable; // Array of memory pointers

program_local void ExitStringAPI() {
	if(IsValid(stringTable) == true) {
		// Free all pointers contained within the table
		ui32 addresses = stringTable.size / sizeof(void*);
		ForAll(addresses) {
			void** table = (void**)stringTable.memory;
			void*  address = table[it];
			Win32FreeMemory(address); // Assumes no pointers have been freed.
		}
		
		FreeStack(stringTable);
	}
}

// ******************** Local API end ******************** //

exported_function char& string::operator[] (ui32 i) {
	AssertRetType(i < this->length, this->chars[0]);
  return this->chars[i];
}

exported_function string::string() {
  if(IsValid(stringTable) == false) { // Init table
	  stringTable = AllocateStack(KiB(1));
		atexit(ExitStringAPI); // @TODO - Returns 0 for no error. What to do if it doesn't?
	}
		
	this->chars = Null;
	this->length = Null;
}

exported_function string::string(const char* s) : string() {
  AssertRet(s != Null);
	
	auto length = GetStringLength(s, true);
	AssertRet(length > 1);
	
	void* mem = Win32AllocateMemory(length);
	CopyMemory((void*)s, length, mem);
	
	PushData(&mem, sizeof(void*), stringTable);
		
	this->chars = (char*)mem;
	this->length = length - 1; // Exclude EOS
}

exported_function string::operator char*() {
	return this->chars;
};

exported_function void string::operator= (const char* s) {
	*this = string(s);
}

exported_function ui16 GetStringLength(const char* s, bool includeEOS) {
  AssertRetType(s != Null, Null);
  
  char c;
  ui64 counter = 0;
  
  do	 	c = s[counter++];
  while (c != '\0');
	
	ui64 fullLength = includeEOS ? counter : counter - 1;
	AssertRetType(fullLength<= UI16Max, Null);
  
  return fullLength;
};

exported_function string ToString(si8 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hhi", i);
	return string((char*)buffer);
}

exported_function string ToString(ui8 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hhu", i);
	return string((char*)buffer);
}

exported_function string ToString(si16 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hi", i);
	return string((char*)buffer);
}

exported_function string ToString(ui16 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%hu", i);
	return string((char*)buffer);
}

exported_function string ToString(si32 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%li", i);
	return string((char*)buffer);
}

exported_function string ToString(ui32 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%lu", i);
	return string((char*)buffer);
}

exported_function string ToString(si64 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%lli", i);
	return string((char*)buffer);
}

exported_function string ToString(ui64 i) {
  char buffer[32] = { '\0' };
  sprintf(buffer, "%llu", i);
	return string((char*)buffer);
}

exported_function string ToString(f32 f) {
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2f", f);
	return string((char*)buffer);
}

exported_function string ToString(f64 f) {
	char buffer[32] = { '\0' };
  sprintf(buffer, "%.2Lf", f);
	return string((char*)buffer);
}

exported_function const char* PushString(const char* string, bool includeEOS, memory_block& stack) {
  auto length = GetStringLength(string, includeEOS);
	if(ErrorIsSet() == true)
		return Null;
	void* mem = PushData((void*)string, length, stack);
	return (const char*)mem;
}

exported_function bool StringsAreEqual(const char* s1, const char* s2) {
  AssertRetType(s1 != Null, false);
	AssertRetType(s2 != Null, false);
	
  ui16 counter = 0;
  do {
    char c1 = s1[counter];
    char c2 = s2[counter];
		if(c1 == '\0' || c2 == '\0') {
			if(c1 == c2)
				return true;
			else
				return false;
		}
		else if(c1 != c2)
			return false;

		counter += 1;
  }
  while(true);
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

exported_function void CopyString(const char* source, si16 srcLength, char* destination, ui16 destLength, bool copyEOS) {
	AssertRet(source != Null);
	AssertRet(srcLength > 0 || srcLength == -1);
	AssertRet(destination != Null);
	AssertRet(destLength > 0);
	
	auto realSourceLength = srcLength == -1 ? GetStringLength(source, true) : srcLength;
	AssertRet(realSourceLength <= destLength);
	
	ForAll(realSourceLength)
	  destination[it] = source[it];
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

exported_function string ExtractSubstring(const char* s, si8 count) {
	AssertRetType(count != 0, string());
	
	string ret(s);
	if(count != -1 && count < ret.length) {
		ret.chars[count] = '\0';
		ret.length = count;
	}
	
	return ret;
}
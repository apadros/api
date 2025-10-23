#include <stdio.h>
#include <string.h>
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_memory.h"
#include "apad_string.h"

exported_function char& short_string::operator[] (ui8 i) {
	AssertRetType(i < ShortStringMaxLength, this->string[0]);
  return this->string[i];
}

exported_function short_string::short_string() {
	ForAll(ShortStringMaxLength)
		this->string[it] = '\0';
}

exported_function short_string::short_string(const char* s) : short_string() {
	CopyString(s, GetStringLength(s, false), this->string, ShortStringMaxLength, false);
}

exported_function short_string::operator char*() {
	return (char*)this->string;
};

exported_function void short_string::operator= (const char* s) {
	*this = short_string(s);
}

exported_function ui16 GetStringLength(const char* s, bool includeEOS) {
  AssertRetType(s != Null, Null);
  
  char c;
  ui16 counter = 0;
  
  do	 	c = s[counter++];
  while (c != '\0');
  
  return includeEOS ? counter : counter - 1;
};

exported_function short_string ToString(si8 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%hhi", i);
	return ret;
}

exported_function short_string ToString(ui8 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%hhu", i);
	return ret;
}

exported_function short_string ToString(si16 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%hi", i);
	return ret;
}

exported_function short_string ToString(ui16 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%hu", i);
	return ret;
}

exported_function short_string ToString(si32 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%li", i);
	return ret;
}

exported_function short_string ToString(ui32 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%lu", i);
	return ret;
}

exported_function short_string ToString(si64 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%lli", i);
	return ret;
}

exported_function short_string ToString(ui64 i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%llu", i);
	return ret;
}

exported_function short_string ToString(f32 f) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%.2f", f);
	return ret;
}

exported_function short_string ToString(f64 f) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%.2Lf", f);
	return ret;
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

exported_function short_string ExtractSubstring(const char* string, si8 count) {
	AssertRetType(count <= ShortStringMaxLength, short_string());
	
	auto stringLength = GetStringLength(string, false);
	ui16 copyLength = count == -1 ? stringLength : Min(stringLength, count);
	
	short_string ret = {};
	CopyString(string, copyLength, ret.string, ShortStringMaxLength, false);
	
	return ret;
}
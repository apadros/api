#include <stdio.h>
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_string.h"

exported_function ui16 GetStringLength(const char* s, bool includeEOS) {
  AssertRet(s != Null, Null);
  
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
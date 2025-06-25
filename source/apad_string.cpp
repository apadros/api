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

#include <stdio.h>
exported_function short_string I64ToString(int i) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%i", i);
	return ret;
}

#include <stdio.h>
exported_function short_string F32ToString(f32 f) {
  short_string ret;
	ClearStruct(ret);
	sprintf(ret.string, "%.2f", f);
	return ret;
}

exported_function const char* PushString(const char* string, memory_block& stack) {
  auto length = GetStringLength(string, true);
	if(ErrorIsSet() == true)
		return Null;
	void* mem = PushData((void*)string, length, stack);
	return (const char*)mem;
}
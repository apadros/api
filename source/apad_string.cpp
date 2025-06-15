#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"

exported_function ui16 GetStringLength(const char* s, bool includeEOS) {
  AssertRet(s != Null, Null);
  
  char c;
  ui16 counter = 0;
  
  do	 	c = s[counter++];
  while (c != '\0');
  
  return includeEOS ? counter : counter - 1;
};
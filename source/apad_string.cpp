#include "apad_base_types.h"
#include "apad_intrinsics.h"

// @TODO - Internal error setting everywhere

exported_function ui16 GetStringLength(const char* s, bool includeEOS) {
  // Assert(s != Null);
	if(s == Null)
		return Null;
  
  char c;
  ui16 counter = 0;
  
  do	 	c = s[counter++];
  while (c != '\0');
  
  return includeEOS ? counter : counter - 1;
};
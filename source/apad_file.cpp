#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_string.h"

dll_export const char* GetFileNameAndExtension(const char* path) {
	AssertRetType(path != Null, Null);
  
  const char* fileName = path + GetStringLength(path);
  do 		 fileName -= 1;
  while (fileName[0] != '\\' && fileName[0] != '/');
  return fileName + 1;
}
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_string.h"

// ******************** Local API start ******************** //



// ******************** Local API end ******************** //

exported_function void* Win32AllocateMemory(ui32 size) {
  void* errorRet = Null;
	
  Assert(size > 0);
	if(ErrorIsSet() == true)
		return errorRet;
  
  void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  Assert(mem != NULL);
	if(ErrorIsSet() == true)
		return errorRet;
	
  return mem;
}

exported_function void Win32FreeMemory(void* mem) {
	Assert(mem != Null);
	if(ErrorIsSet() == true)
		return;
	
  auto ret = VirtualFree(mem, 0, MEM_RELEASE);
	Assert(ret != 0);
}

exported_function bool Win32FileExists(const char* path) {
	AssertRet(path != Null, false);
	AssertRet(GetStringLength(path, true) <= MAX_PATH, false);
	
	HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  auto error = GetLastError();
  if (handle == INVALID_HANDLE_VALUE && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND))
    return false;

  CloseHandle(handle);
  return true;
}

exported_function memory_block Win32LoadFile(const char* path) {
	AssertRet(Win32FileExists(path), NullMemoryBlock);
	
  HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	Assert(handle != INVALID_HANDLE_VALUE);
	if(ErrorIsSet() == true) {
    auto error = GetLastError();
		AssertRet(error != ERROR_FILE_NOT_FOUND, NullMemoryBlock);
		AssertRet(error != ERROR_PATH_NOT_FOUND, NullMemoryBlock);
		AssertRet(error != ERROR_SHARING_VIOLATION, NullMemoryBlock);
		// @TODO - More logging
		return NullMemoryBlock;
	}

  LARGE_INTEGER li = {};
  BOOL b = GetFileSizeEx(handle, &li);
  Assert(b != 0);
	if(ErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  Assert(li.QuadPart <= 0xFFFFFFFF);
	if(ErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  
  DWORD fileSize = (DWORD)li.QuadPart;
  auto  allocatedMemory = AllocateMemory((ui32)fileSize);
  DWORD bytesRead = 0;
  b = ReadFile(handle, allocatedMemory.memory, fileSize, &bytesRead, NULL); // If it fails, returns FALSE
  Assert(b == TRUE);
  if(ErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  Assert(bytesRead == fileSize);
	if(ErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}

	CloseHandle(handle);
  return allocatedMemory;
}

exported_function void Win32SaveFile(void* data, ui32 dataSize, const char* path) {
  Assert(data != Null);
	if(ErrorIsSet() == true)
		return;
	Assert(dataSize > 0);
	if(ErrorIsSet() == true)
	return;

  HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	Assert(handle != INVALID_HANDLE_VALUE);
	if(ErrorIsSet() == true) {
		auto error = GetLastError();
    // @TODO - Logging
		return;
  }
	
  DWORD written = 0;
  BOOL result = WriteFile(handle, data, dataSize, &written, NULL);
	Assert(result != FALSE);
	if(ErrorIsSet() == true) {
    auto error = GetLastError();
		// @TODO - Logging
		CloseHandle(handle);
		return;
  }
  
  CloseHandle(handle); // No need to assert, handle will be closed on program exit at the latest
}
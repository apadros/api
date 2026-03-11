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

#include <dbghelp.h>
#include <stdio.h> // For sprintf()
#include "apad_array.h"
#include "apad_file.h"
#include "apad_string.h"
// No assertions calls in this function since it is a part of the Assert() macros
dll_export void Win32PrintStackBackTrace() {
  // Capture stack back trace
  PVOID stacktrace[32] = {}; // We assume we won't need more than 32 stack frames
  USHORT depth = CaptureStackBackTrace(1 /* Skip the call to Win32PrintStackBackTrace() */, GetArrayLength(stacktrace), stacktrace, NULL);
  if(depth == 0) {
		SetGlobalError(Concatenate(2, "CaptureStackBackTrace() failed in Win32PrintStackBackTrace(), code %i.", ToString((ui32)GetLastError())));
		DisplayGlobalError();
		return;
  }
	
	InvalidCodePath; // Actually log / display back trace
  
  // Get an initialised handle for the current process
  HANDLE process = {};
  {
    HANDLE pseudoHandle = GetCurrentProcess();
    BOOL   ret = DuplicateHandle(pseudoHandle, pseudoHandle, pseudoHandle, &process, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if(ret == 0) { // DuplicateHandle failed
			SetGlobalError(Concatenate(2, "DuplicateHandle() failed in Win32PrintStackBackTrace(), code %i.", ToString((ui32)GetLastError())));
			DisplayGlobalError();
			return;
    }
    
    ret = SymInitialize(process, NULL, TRUE);
    if(ret == FALSE) {
      SetGlobalError("SymInitialize() failed in Win32PrintStackBackTrace().");
			DisplayGlobalError();
      return;
    }
  }

  // Set the symbol handler options
  {
    DWORD options = SYMOPT_UNDNAME | SYMOPT_LOAD_LINES;
		GetLastError(); // Reset system error
    DWORD ret = SymSetOptions(options);
		auto error = GetLastError();
		if(ret != options || error != ERROR_SUCCESS) {
			if(error == ERROR_SUCCESS) {
				SetGlobalError("SymSetOptions() failed in Win32PrintStackBackTrace(), returned mask does not match request.");
				DisplayGlobalError();
			}
			else {
				SetGlobalError("SymSetOptions() failed in Win32PrintStackBackTrace(), unknow error.");
				DisplayGlobalError();
			}
			
			SymCleanup(process);
			return;
		}
  }

  // Print log
  const ui8 bufferSize = UI8Max;
  char previousSymbolName[bufferSize] = {};
  ForAll(depth) {
    if(depth >= GetArrayLength(stacktrace)) {
			SymCleanup(process);
			return;
		}

    DWORD64 address = (DWORD64)(stacktrace[it]);
	
    // Function name
    char buffer[sizeof(SYMBOL_INFO) + bufferSize * sizeof(TCHAR)] = {};
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = bufferSize;
    DWORD64 displacement64 = Null;
    BOOL ret = SymFromAddr(process, address, (PDWORD64)(&displacement64), pSymbol);
		if(ret == FALSE) {
			SetGlobalError(Concatenate(2, "SymFromAddr() failed in Win32PrintStackBackTrace(), code %i.\n", ToString((ui32)GetLastError())));
			DisplayGlobalError();
      SymCleanup(process);
			return;
    }

    // File name and line
    IMAGEHLP_LINE64 fileLine = {};
    fileLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    DWORD displacement = Null;
    ret = SymGetLineFromAddr64(process, address, &displacement /* Contrary to msdn documentation, Null / 0 doesn't work here. */, &fileLine);
    if(ret == FALSE) {
      SetGlobalError(Concatenate(2, "SymGetLineFromAddr64() failed in Win32PrintStackBackTrace(), code %i.\n", ToString((ui32)GetLastError())));
			DisplayGlobalError();
      continue; // Could be windows stuff in between functions for some reason, like during window proc calls
    }

    // Log all
		const char* finalString = Null;
		if(it == 0)
			finalString = Concatenate(3, "  %s %i\n", GetFileNameAndExtension(fileLine.FileName), ToString((ui32)fileLine.LineNumber));
		else
			finalString = Concatenate(5, finalString, "  %s %i -> %s()\n", GetFileNameAndExtension(fileLine.FileName), fileLine.LineNumber, previousSymbolName);
		ClearArray(previousSymbolName);
		CopyMemory(pSymbol->Name, bufferSize, previousSymbolName);

    // Stop after WinMain
    if(StringsAreEqual(pSymbol->Name, "WinMain") == true || StringsAreEqual(pSymbol->Name, "main")) {
			SetGlobalError(finalString);
			DisplayGlobalError();
      break;
		}
  }
	
	SymCleanup(process);
}

dll_export void* Win32AllocateMemory(ui32 size) {
	AssertRetType(size > 0, Null);
	
  void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  AssertRetType(mem != NULL, Null);
	
  return mem;
}

dll_export void Win32FreeMemory(void* mem) {
	AssertRet(mem != Null);
	
  auto ret = VirtualFree(mem, 0, MEM_RELEASE);
	Assert(ret != 0);
}

dll_export bool Win32FileExists(const char* path) {
	AssertRetType(path != Null, false);
	AssertRetType(GetStringLength(path) + 1 <= MAX_PATH, false);
	
	HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// @TODO - Can this be layed out better?
	// @TODO - Logging or displaying of the error somehow
  auto error = GetLastError();
  if (handle == INVALID_HANDLE_VALUE && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND))
    return false;

  CloseHandle(handle);
  return true;
}

dll_export memory_block Win32LoadFile(const char* path) {
	AssertRetType(Win32FileExists(path), NullMemoryBlock);
	
  HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	Assert(handle != INVALID_HANDLE_VALUE);
	if(GlobalErrorIsSet() == true) {
		// @TODO - Can all of this be improved / layed out better?
		// @TODO - Logging or displaying of the error somehow
    auto error = GetLastError();
		AssertRetType(error != ERROR_FILE_NOT_FOUND, NullMemoryBlock);
		AssertRetType(error != ERROR_PATH_NOT_FOUND, NullMemoryBlock);
		AssertRetType(error != ERROR_SHARING_VIOLATION, NullMemoryBlock);
		return NullMemoryBlock;
	}

  LARGE_INTEGER li = {};
  BOOL b = GetFileSizeEx(handle, &li);
  Assert(b != 0);
	if(GlobalErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  Assert(li.QuadPart <= 0xFFFFFFFF);
	if(GlobalErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  
  DWORD fileSize = (DWORD)li.QuadPart;
  auto  allocatedMemory = AllocateMemory((ui32)fileSize);
  DWORD bytesRead = 0;
  b = ReadFile(handle, allocatedMemory.memory, fileSize, &bytesRead, NULL); // If it fails, returns FALSE
  Assert(b == TRUE);
  if(GlobalErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}
  Assert(bytesRead == fileSize);
	if(GlobalErrorIsSet() == true) {
		CloseHandle(handle);
		return NullMemoryBlock;
	}

	CloseHandle(handle);
  return allocatedMemory;
}

dll_export void Win32SaveFile(void* data, ui32 dataSize, const char* path) {
  AssertRet(data != Null);
	AssertRet(dataSize > 0);
	
  HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	Assert(handle != INVALID_HANDLE_VALUE);
	if(GlobalErrorIsSet() == true) {
		auto error = GetLastError();
    // @TODO - Logging / Displaying of error in GUI program
		return;
  }
	
  DWORD written = 0;
  BOOL result = WriteFile(handle, data, dataSize, &written, NULL);
	Assert(result != FALSE);
	if(GlobalErrorIsSet() == true) {
    auto error = GetLastError();
		// @TODO - Logging / Displaying of error in GUI program
		CloseHandle(handle);
		return;
  }
  
  CloseHandle(handle); // No need to assert, handle will be closed on program exit at the latest
}

#include "apad_time.h"
dll_export time_marker Win32GetTimeMarker() {
	// Init cpuCounterFrequencyKHz if it hasn't already
	{
		dll_import program_external ui64 cpuCounterFrequencyKHz;
		if(cpuCounterFrequencyKHz == Null) {
			LARGE_INTEGER temp = {};
			AssertRetType(QueryPerformanceFrequency(&temp) != 0, Null);
			cpuCounterFrequencyKHz = temp.QuadPart;
			AssertRetType(cpuCounterFrequencyKHz != Null, Null);
		}
	}
	
  LARGE_INTEGER temp = {};
  AssertRetType(QueryPerformanceCounter(&temp) != 0, 0);
  return temp.QuadPart; // Value in kilo counts
}
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include "apad_base_types.h"
#include "apad_error_internal.h"
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
// No assertions calls in this function since it is a part of both the internal and external assertion macros
dll_export void Win32PrintStackBackTrace() {
	FunctionStart(;);
	
	GetLastError(); // Reset system error
  
	// Capture stack back trace
  PVOID stacktrace[32] = {}; // We assume we won't need more than 32 stack frames
  USHORT depth = CaptureStackBackTrace(1 /* Skip the call to Win32PrintStackBackTrace() */, GetArrayLength(stacktrace), stacktrace, NULL);
  if(depth == 0) {
		DisplayError(Concatenate(2, "CaptureStackBackTrace() failed in Win32PrintStackBackTrace(), Windows code ", ToString((ui32)GetLastError())));
			FunctionEnd();
		return;
  }
	
	// Get an initialised handle for the current process
  HANDLE process = {};
  {
    HANDLE pseudoHandle = GetCurrentProcess();
    BOOL   ret = DuplicateHandle(pseudoHandle, pseudoHandle, pseudoHandle, &process, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if(ret == 0) { // DuplicateHandle failed
			DisplayError(Concatenate(2, "DuplicateHandle() failed in Win32PrintStackBackTrace(), Windows code ", ToString((ui32)GetLastError())));
			FunctionEnd();
			return;
    }
    
    ret = SymInitialize(process, NULL, TRUE);
    if(ret == FALSE) {
      DisplayError(Concatenate(2, "SymInitialize() failed in Win32PrintStackBackTrace(), Windows code ", ToString((ui32)GetLastError())));
			FunctionEnd();
      return;
    }
  }

  // Set the symbol handler options
  {
    DWORD options = SYMOPT_UNDNAME | SYMOPT_LOAD_LINES;
		DWORD ret = SymSetOptions(options);
		auto error = GetLastError();
		if(ret != options || error != ERROR_SUCCESS) {
			if(error == ERROR_SUCCESS)
				DisplayError("SymSetOptions() failed in Win32PrintStackBackTrace(), returned mask does not match request.");
			else
				DisplayError("SymSetOptions() failed in Win32PrintStackBackTrace(), unknow error.");
			
			SymCleanup(process);
			FunctionEnd();
			return;
		}
  }

  // Print log
  const ui8 bufferSize = UI8Max;
  char previousSymbolName[bufferSize] = {};
	const char* finalString = Null;
  ForAll(depth) {
    if(depth >= GetArrayLength(stacktrace)) {
			SymCleanup(process);
			FunctionEnd();
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
			DisplayError(Concatenate(2, "SymFromAddr() failed in Win32PrintStackBackTrace(), Windows code ", ToString((ui32)GetLastError())));
			SymCleanup(process);
			FunctionEnd();
			return;
    }

    // File name and line
    IMAGEHLP_LINE64 fileLine = {};
    fileLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    DWORD displacement = Null;
    ret = SymGetLineFromAddr64(process, address, &displacement, &fileLine);
    if(ret == FALSE) {
      DisplayError(Concatenate(2, "SymGetLineFromAddr64() failed in Win32PrintStackBackTrace(), Windows code ", ToString((ui32)GetLastError())));
			continue; // Could be windows stuff in between functions for some reason, like during window proc calls
    }

    // Log all
		if(it == 0)
			finalString = Concatenate(4, "  Stack back trace\n    [", GetFileNameAndExtension(fileLine.FileName), "] line ", ToString((ui32)fileLine.LineNumber));
		else
			finalString = Concatenate(8, finalString, "\n    [", GetFileNameAndExtension(fileLine.FileName), "] line ", ToString((ui32)fileLine.LineNumber), " -> ", previousSymbolName, "()");
		ClearArray(previousSymbolName);
		CopyMemory(pSymbol->Name, bufferSize, previousSymbolName);

    // Stop after WinMain
    if(StringsAreEqual(pSymbol->Name, "WinMain") == true || StringsAreEqual(pSymbol->Name, "main")) {
			DisplayError(finalString);
			break;
		}
  }
	
	SymCleanup(process);
	
	FunctionEnd();
}

dll_export void* Win32AllocateMemory(ui32 size) {
	FunctionStart(Null);
  AssertInternal(size > 0);
	
  void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  AssertInternal(mem != NULL);
	
	FunctionEnd();
  return mem;
}

dll_export void Win32FreeMemory(void* mem) {
	FunctionStart(;);
  AssertInternal(mem != Null);
	
  auto ret = VirtualFree(mem, 0, MEM_RELEASE);
	AssertInternal(ret != 0);
	
	FunctionEnd();
}

dll_export bool Win32FileExists(const char* path) {
	FunctionStart(false);
  AssertInternal(path != Null);
	AssertInternal(GetStringLength(path) + 1 <= MAX_PATH);
	
	HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		FunctionEnd();
		return false;
	}
	
	// Leave this here for now if more info wanted
	#if 0
	auto error = GetLastError();
  if (handle == INVALID_HANDLE_VALUE && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)) {
		// Store the assertion string
		AssertInternal(handle != INVALID_HANDLE_VALUE);
		AssertInternal(error != ERROR_FILE_NOT_FOUND);
		AssertInternal(error != ERROR_PATH_NOT_FOUND);
	}
	#endif

  CloseHandle(handle);
	
	FunctionEnd();
  return true;
}

dll_export memory_block Win32LoadFile(const char* path) {
	FunctionStart(memory_block());
  AssertInternal(Win32FileExists(path));
	
  HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	AssertInternal(handle != INVALID_HANDLE_VALUE);
	
	#if 0
	if(handle == INVALID_HANDLE_VALUE) {
		// @TODO - Can all of this be improved / layed out better?
		// @TODO - Logging or displaying of the error somehow
    auto error = GetLastError();
		AssertRetType(error != ERROR_FILE_NOT_FOUND, NullMemoryBlock);
		AssertRetType(error != ERROR_PATH_NOT_FOUND, NullMemoryBlock);
		AssertRetType(error != ERROR_SHARING_VIOLATION, NullMemoryBlock);
		FunctionEnd();
		return NullMemoryBlock;
	}
	#endif
	
  LARGE_INTEGER li = {};
  BOOL b = GetFileSizeEx(handle, &li);
  if(b == 0) {
		CloseHandle(handle);
		AssertInternal(false);
	}
  if(li.QuadPart > 0xFFFFFFFF) {
		CloseHandle(handle);
		AssertInternal(false);
	}
  
  DWORD fileSize = (DWORD)li.QuadPart;
  auto  allocatedMemory = AllocateMemory((ui32)fileSize);
  DWORD bytesRead = 0;
  b = ReadFile(handle, allocatedMemory.memory, fileSize, &bytesRead, NULL); // If it fails, returns FALSE
  Assert(b == TRUE);
  if(b != TRUE) {
		CloseHandle(handle);
		AssertInternal(false);
	}
  if(bytesRead != fileSize) {
		CloseHandle(handle);
		AssertInternal(false);
	}

	CloseHandle(handle);
	
	FunctionEnd();
  return allocatedMemory;
}

dll_export void Win32SaveFile(void* data, ui32 dataSize, const char* path) {
  FunctionStart(;);
  AssertInternal(data != Null);
	AssertInternal(dataSize > 0);
	
  HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	AssertInternal(handle != INVALID_HANDLE_VALUE);
	// @TODO - Handle this better, possibly return without an assertion to allow calling code to handle failure to save
	
  DWORD written = 0;
  BOOL result = WriteFile(handle, data, dataSize, &written, NULL);
	if(result == FALSE) {
	  auto error = GetLastError();
		// @TODO - Logging / Displaying of error in GUI program
  }
  
  CloseHandle(handle); // No need to assert, handle will be closed on program exit at the latest
	
	FunctionEnd();
}

#include "apad_time.h"
dll_export time_marker Win32GetTimeMarker() {
	FunctionStart(0);
	
  // Init cpuCounterFrequencyKHz if it hasn't already
	{
		dll_import program_external ui64 cpuCounterFrequencyKHz;
		if(cpuCounterFrequencyKHz == Null) {
			LARGE_INTEGER temp = {};
			AssertInternal(QueryPerformanceFrequency(&temp) != 0);
			cpuCounterFrequencyKHz = temp.QuadPart;
			AssertInternal(cpuCounterFrequencyKHz != Null);
		}
	}
	
  LARGE_INTEGER temp = {};
  AssertInternal(QueryPerformanceCounter(&temp) != 0);
  auto ret = temp.QuadPart; // Value in kilo counts
	
	FunctionEnd();
	return ret;
}
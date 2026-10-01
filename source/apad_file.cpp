#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_file.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_string.h"

dll_export void SaveFile(file& f, const char* path) {
	FunctionStart(;);
	AssertInternal(path != Null);
	auto size = GetSize(f);
	SaveFile(f.memory.memory, size, path);
	FunctionEnd();
}

dll_export void SaveFile(void* data, ui32 dataSize, const char* path) {
	FunctionStart(;);
	Win32SaveFile(data, dataSize, path); // For some reason have to call this here instead of defining SaveFile() as a pointer to Win32SaveFile() when having an overload of SaveFile()
	FunctionEnd();
}

dll_export const char* GetFileNameAndExtension(const char* path) {
	FunctionStart(Null);
	AssertInternal(path != Null);
  
  const char* fileName = path + GetLength(path);
  do 		 fileName -= 1;
  while (fileName[0] != '\\' && fileName[0] != '/');
  return fileName + 1;
	
	FunctionEnd();
}

dll_export const char* GetFileExtension(const char* path) {
	FunctionStart(Null);
	AssertInternal(path != Null);
  
  const char* fileName = path + GetLength(path);
  do 		 fileName -= 1;
  while (fileName[0] != '.');
  return fileName + 1;
	
	FunctionEnd();
}

dll_export file_line ParseLine(file& f, ui32& readIndex) {
	FunctionStart(file_line());
	
	auto fileSize = GetSize(f);
	if(readIndex >= fileSize)
		return file_line();
	
	char*        string = Null;
	bool         readingData = false;
	memory_stack stack = AllocateStack();
	for(char* c = ((char*)f.memory.memory) + readIndex; readIndex < fileSize; c = ((char*)f.memory.memory) + ++readIndex) {
		if(*c == '"') { // Read / store string between quotation marks
			AssertInternal(readingData == false); // In case a white space is missing between the end of data and the start of a string
			
			if(string == Null) {
				if(readIndex + 1 >= fileSize) // If, for whatever reason, we're at the very end of the file
					break;
				string = c + 1;
			}
			else {
				Push((void*)&string, sizeof(char*), stack);
				string = Null;
			}
			*c = '\0';
		}
		else if(string != Null) // Currently reading a string
			continue;
		else if(IsWhitespace(*c) == true) {
			bool end = *c == '\n';
			*c = '\0';
			readingData = false;
			if(end == true) {
				readIndex += 1;
				break;
			}
		}
		else if(readingData == false) { // Other data to be read
		  Push((void*)&c, sizeof(char*), stack);
			readingData = true;
		} 
	}
	
	file_line ret = {};
	ret.data = stack;
	ret.count = stack.size / sizeof(char*);
	
	FunctionEnd();
	return ret;
}

dll_export bool LineIsValid(file_line& f) {
	return f.data.size > 0 && f.count > 0;
}

dll_export void FreeLine(file_line& line) {
	FunctionStart(;);
	Free(line.data);
	FunctionEnd();
}

dll_export char* GetLineDataElement(file_line& line, ui8 index) {
	FunctionStart(Null);
	AssertInternal(line.data.size > 0);
	AssertInternal(line.data.size % sizeof(char*) == 0);
	AssertInternal(index < line.data.size / sizeof(char*));
	
	auto ret = ((char**)line.data.memory.memory)[index];
	
	FunctionEnd();
	return ret;
}

dll_export file CreateFile() {
	FunctionStart(file());
	auto ret = AllocateStack();
	FunctionEnd();
	return ret;
}

dll_export void WriteToFile(void* data, ui32 size, file& f) {
	FunctionStart(;);
	Push(data, size, f);
	FunctionEnd();
}

dll_export void WriteToFile(char* string, file& f) {
	FunctionStart(;);
	Push(string, false, f);
	FunctionEnd();
}

dll_export void* GetMemory(file& f) {
	FunctionStart(Null);
	auto ret = f.memory.memory;
	FunctionEnd();
	return ret;
}

dll_export ui32 GetSize(file& f) {
	FunctionStart(Null);
	
	ui32 ret = Null;
	if(f.memory.size == 0) { // Opened for reading, capacity == 0
		AssertInternal(IsValid(f.memory) == true);
		ret = f.memory.size;
	}
	else {
		AssertInternal(IsValid(f) == true);
		ret = f.memory.size;
	}
	
	FunctionEnd();
	return ret;
}

dll_export file LoadFile(const char* path) {
	FunctionStart(file());
	AssertInternal(path != Null);
	
	file ret = {};
	
	auto memory = Win32LoadFile(path);
	if(IsValid(memory) == true) { // File load successful
		ret.memory = memory;
		ret.size = 0;
	}
	
	FunctionEnd();
	return ret;
}
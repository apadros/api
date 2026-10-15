#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_file.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_string.h"
#include "apad_string_internal.h"

dll_export void SaveFile(file& f, string_args(path)) {
	FunctionStart(;);
	
	auto size = GetSize(f);
	SaveFile(f.memory.memory, size, UnpackStringArgs(path));
	
	FunctionEnd();
}

dll_export void SaveFile(void* data, ui32 dataSize, string_args(path)) {
	FunctionStart(;);
	Win32SaveFile(data, dataSize, UnpackStringArgs(path)); // For some reason have to call this here instead of defining SaveFile() as a pointer to Win32SaveFile() when having an overload of SaveFile()
	FunctionEnd();
}

dll_export char* GetFileNameAndExtension(string_args(path)) {
	FunctionStart(Null);
	CheckStringInternal(UnpackStringArgs(path));
	
  char* fileName = path + GetLength(path);
	char* ret = Null;
  do 		 fileName -= 1;
  while (fileName[0] != '\\' && fileName[0] != '/' && fileName > path); // The latter in case there is no \ or /
	if(fileName == path) // No \ or / found
		ret = fileName;
	else
		ret = fileName + 1;
	
	FunctionEnd();
	return ret;
}

dll_export char* GetFileExtension(string_args(path)) {
	FunctionStart(Null);
	CheckStringInternal(UnpackStringArgs(path));
	
  char*  fileName = path + stringArgsLength(path);
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

dll_export bool IsValid(file_line& f) {
	return IsValid(f.data) && f.count > 0;
}

dll_export void Free(file_line& line) {
	FunctionStart(;);
	Free(line.data);
	line.count = Null;
	FunctionEnd();
}

dll_export char* GetDataElement(file_line& line, ui8 index) {
	FunctionStart(Null);
	AssertInternal(line.data.size > 0);
	AssertInternal(line.data.size % sizeof(char*) == 0);
	AssertInternal(index < line.data.size / sizeof(char*));
	
	auto ret = ((char**)GetMemory(line.data))[index];
	
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

dll_export void WriteToFile(string_args(string), file& f) {
	FunctionStart(;);
	Push(UnpackStringArgs(string), false, f);
	FunctionEnd();
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

dll_export file LoadFile(string_args(path)) {
	FunctionStart(file());
	AssertInternal(path != Null);
	
	file ret = {};
	
	auto memory = Win32LoadFile(UnpackStringArgs(path));
	if(IsValid(memory) == true) { // File load successful
		ret.memory = memory;
		ret.size = 0;
	}
	
	FunctionEnd();
	return ret;
}
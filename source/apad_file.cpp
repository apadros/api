#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_string.h"

dll_export const char* GetFileNameAndExtension(const char* path) {
	FunctionStart(Null);
	AssertInternal(path != Null);
  
  const char* fileName = path + GetStringLength(path);
  do 		 fileName -= 1;
  while (fileName[0] != '\\' && fileName[0] != '/');
  return fileName + 1;
	
	FunctionEnd();
}

dll_export file_line ReadLine(file& f, ui32& readIndex) {
	FunctionStart(file_line());
	
	if(readIndex >= f.size)
		return file_line();
	
	file_line line = { Null };
	
	char* string = Null;
	bool  readingData = false;
	for(char* c = ((char*)f.memory) + readIndex; readIndex < f.size; c = ((char*)f.memory) + ++readIndex) {
		if(*c == '"') { // Read / store string between quotation marks
			AssertInternal(readingData == false); // In case a white space is missing between the end of data and the start of a string
			
			if(string == Null) {
				if(readIndex + 1 >= f.size) // If, for whatever reason, we're at the very end of the file
					break;
				string = c + 1;
			}
			else {
				AssertInternal(line.count < MaxFileLineElements);
				line.data[line.count++] = string;
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
			AssertInternal(line.count < MaxFileLineElements);
			line.data[line.count++] = c;
			readingData = true;
		} 
	}
	
	FunctionEnd();
	return line;
}

dll_export bool LineIsValid(file_line& f) {
	return f.data[0] != Null;
}
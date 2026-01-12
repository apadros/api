#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"

// ******************** Imported libs ******************** //


// ******************** Local API start ******************** //

program_local const ui8  ErrorStringMaxLength = UI8Max;
program_local 			char ErrorString[ErrorStringMaxLength] = {};
program_local 			bool ExitIfError = false;

program_local 			bool PrintAssertions = true;

// ******************** Local API end ******************** //

exported_function bool IsAssertionPrintingSet() {
	return PrintAssertions;
}

exported_function void SetAssertionPrinting(bool b) {
  PrintAssertions = b;
}

exported_function void SetError(const char* string) {
	// Since this function will be called everywhere, avoid calling external code 
	// within it to avoid potential infinite loops

  if(string == Null)
		return;
	
  // Code copied from GetStringLength() in  apad_string.cpp
	char c;
  ui16 	sourceLength = 0;
  do {	 	
		c = string[sourceLength++];
		if(sourceLength == ErrorStringMaxLength - 1) // Couldn't find an EOS character
			return;
	}
  while (c != '\0');
	
	ForAll(sourceLength)
	  ErrorString[it] = string[it];
}

exported_function bool ErrorIsSet() {
  return ErrorString[0] != '\0';
}

exported_function void ClearError() {
  ErrorString[0] = '\0';
}

exported_function const char* GetError() {
	return ErrorString;
}

#include <stdlib.h> // For exit()
exported_function void ExitProgram(bool error) {
	if(error == true)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

exported_function void SetExitIfError(bool b) {
  ExitIfError = b;
}

exported_function bool IsExitIfErrorSet() {
	return ExitIfError;
}

exported_function void DisplayErrorGUI(const char* string) {
	MessageBox(NULL, string, "Error", MB_OK | MB_ICONEXCLAMATION);
}
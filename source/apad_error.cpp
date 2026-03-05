#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"

// ******************** Imported libs ******************** //


// ******************** Internal API start ******************** //

program_local 		 const 					 ui8  ErrorStringMaxLength = UI8Max;
program_local 										 char ErrorString[ErrorStringMaxLength] = {};
program_local 										 bool ExitIfError = false;

dll_export program_external bool GUIApp = false; // Checked outside this translation unit

// ******************** Internal API end ******************** //

dll_export void SetError(const char* string) {
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

dll_export bool ErrorIsSet() {
  return ErrorString[0] != '\0';
}

dll_export void ClearError() {
  ErrorString[0] = '\0';
}

dll_export const char* GetError() {
	return ErrorString;
}

#include <stdlib.h> // For exit()
dll_export void ExitProgram(bool error) {
	if(GUIApp == true) {
		dll_import program_external void Win32Exit();
		Win32Exit();
	}
		
	if(error == true)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

dll_export void SetExitIfError(bool b) {
  ExitIfError = b;
}

dll_export bool IsExitIfErrorSet() {
	return ExitIfError;
}

dll_export void DisplayErrorGUI(const char* string) {
	extern void Win32ErrorMessageBox(const char* string);
	Win32ErrorMessageBox(string);
}
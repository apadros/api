#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"

// ******************** Imported libs ******************** //


// ******************** Internal API start ******************** //

					 program_local 	  const ui8 ErrorStringMaxLength = UI8Max;
					 program_local 	 			 char ErrorString[ErrorStringMaxLength] = {};
					 program_local 	 			 bool ExitIfError = true;
								 
					 program_external 		 bool GUIApp = false; // Checked outside this translation unit
					 program_external      bool DisplayAPIAssertions = true;
					 program_external      bool CallExitInAPIAssertions = true;

// ******************** Internal API end ******************** //

#include <stdlib.h>
dll_export void RegisterExitFunction(void (*function)()) {
	atexit(function); // @TODO - Returns 0 for no error. What to do if it doesn't?
}

dll_export void SetDisplayAPIAssertions(bool b) {
	DisplayAPIAssertions = b;
}

#include <stdio.h>
dll_export void DisplayGlobalError() {
	FunctionStart(;);
	
	DisplayError(GetGlobalError());
	
	FunctionEnd();
}

dll_export void SetGlobalError(const char* string) {
	// Avoid assertions here since they call into this function

  if(string == Null)
		return;
	
	ui32 it = 0;
	do { 
		ErrorString[it] = string[it];
		it += 1;
	} while(ErrorString[it - 1] != '\0');
}

dll_export bool GlobalErrorIsSet() {
  return ErrorString[0] != '\0';
}

dll_export void ClearGlobalError() {
  ErrorString[0] = '\0';
}

dll_export const char* GetGlobalError() {
	return ErrorString;
}

#include <stdlib.h> // For exit()
dll_export void ExitProgram(bool error) {
	if(error == true)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

dll_export void SetExitIfAssertionHit(bool b) {
  ExitIfError = b;
}

dll_export bool IsExitIfAssertionHitSet() {
	return ExitIfError;
}

dll_export void DisplayError(const char* string) {
	FunctionStart(;);
	AssertInternal(string != Null);
	
	if(GUIApp == false)
		printf("\n%s\n", string);
	else {
		program_external void Win32ErrorMessageBox(const char* string);
		Win32ErrorMessageBox(string);
	}
	
	FunctionEnd();
}
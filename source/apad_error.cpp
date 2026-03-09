#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"

// ******************** Imported libs ******************** //


// ******************** Internal API start ******************** //

program_local 	 const ui8  ErrorStringMaxLength = UI8Max;
program_local 	 			 char ErrorString[ErrorStringMaxLength] = {};
program_local 	 			 bool ExitIfError = false;
								 
program_external  		 bool GUIApp = false; // Checked outside this translation unit
program_external       bool AssertionsEnabled = true; // Used to disable assertions when called from functions which are in turn called from within an assertion

// ******************** Internal API end ******************** //

#include <stdio.h>
dll_export void DisplayGlobalError() {
	if(GUIApp == false)
		printf("\n%s\n", GetError());
	else {
		program_external void Win32ErrorMessageBox(const char* string);
		Win32ErrorMessageBox(string);
	}
}

dll_export void SetGlobalError(const char* string) {
	// Since this function will be called everywhere, avoid calling external code 
	// within it to avoid potential infinite loops

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
	if(GUIApp == true) {
		dll_import program_external void Win32Exit();
		Win32Exit();
	}
		
	if(error == true)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

dll_export void SetExitIfGlobalError(bool b) {
  ExitIfError = b;
}

dll_export bool IsExitIfGlobalErrorSet() {
	return ExitIfError;
}
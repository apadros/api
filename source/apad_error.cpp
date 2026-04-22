#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"

// ******************** Imported libs ******************** //


// ******************** Internal API start ******************** //

					 program_external bool GUIApp = false;
					 
dll_export program_external bool AssertionHit = false; //Declared as an export since it is used in external assertions
dll_export program_external bool CallExitInExternalAssertion = true; //Declared as an export since it is used in external assertions

					 program_external bool DisplayAPIAssertions = true;
					 program_external bool CallExitInAPIAssertions = true;

#ifdef APAD_ASSERTIONS_BACKTRACE					 
dll_export program_external bool PrintAssertionsBacktrace = true;
#else
dll_export program_external bool PrintAssertionsBacktrace = false;
#endif


// ******************** Internal API end ******************** //

#include <stdlib.h>
dll_export void RegisterExitFunction(void (*function)()) {
	atexit(function); // @TODO - Returns 0 for no error. What to do if it doesn't?
}

dll_export void SetDisplayAPIAssertions(bool b) {
	DisplayAPIAssertions = b;
}

dll_export bool AssertionWasHit() {
  return AssertionHit;
}

dll_export void ClearAssertionHit() {
  AssertionHit = false;
}

#include <stdlib.h> // For exit()
dll_export void ExitProgram(bool error) {
	if(error == true)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

dll_export void SetExitIfAssertionHit(bool b) {
  CallExitInExternalAssertion = b;
}

dll_export void SetCallExitInAPIAssertions(bool b) {
	CallExitInAPIAssertions = b;
}

dll_export program_external void DisplayError(const char* string) {
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
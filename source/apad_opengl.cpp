#include <windows.h>
#include <gl\gl.h>
#include "apad_opengl_internal.h"

dll_export program_external void AssertOpenGL() {
	AssertInternalGL();
}
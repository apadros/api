#ifndef APAD_OPENGL_INTERNAL_H
#define APAD_OPENGL_INTERNAL_H

#include <windows.h>
#include <gl\gl.h>
#include "apad_error_internal.h"

#ifdef APAD_DEBUGGER_ASSERTIONS

#define AssertInternalGL() \
	Assert(glGetError == GL_NO_ERROR)

#else

#define AssertInternalGL() { \
	auto error = glGetError(); \
	BeginInternalAssertion(error == GL_NO_ERROR) \
	if(error == GL_INVALID_ENUM) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_INVALID_ENUM"); \
	else if(error == GL_INVALID_VALUE) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_INVALID_VALUE"); \
	else if(error == GL_INVALID_OPERATION) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_INVALID_OPERATION"); \
	else if(error == GL_STACK_OVERFLOW) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_STACK_OVERFLOW"); \
	else if(error == GL_STACK_UNDERFLOW) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_STACK_UNDERFLOW"); \
	else if(error == GL_OUT_OF_MEMORY) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "GL_OUT_OF_MEMORY"); \
	else if(error != GL_NO_ERROR) \
		sprintf(buffer, "%s\n  [OpenGL error]  %s", (const char*)buffer, "Unknown"); \
	EndInternalAssertion() \
}

#endif

#endif
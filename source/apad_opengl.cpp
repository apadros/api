#include <windows.h>
#include <gl\gl.h>
#include "apad_string.h"

char* CheckOpenGLError() {
	auto error = glGetError();
	if(error == GL_NO_ERROR)
		return Null;
	else if(error == GL_INVALID_ENUM)
		return AllocateString("GL_INVALID_ENUM", Null);
	else if(error == GL_INVALID_VALUE)
		return AllocateString("GL_INVALID_VALUE", Null);
	else if(error == GL_INVALID_OPERATION)
		return AllocateString("GL_INVALID_OPERATION", Null);
	else if(error == GL_STACK_OVERFLOW)
		return AllocateString("GL_STACK_OVERFLOW", Null);
	else if(error == GL_STACK_UNDERFLOW)
		return AllocateString("GL_STACK_UNDERFLOW", Null);
	else if(error == GL_OUT_OF_MEMORY)
		return AllocateString("GL_OUT_OF_MEMORY", Null);
	else
		return AllocateString("Unknown", Null);
}
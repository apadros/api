#include <windows.h>
#include <gl\gl.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_gui.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_opengl.h"

dll_export program_external text_body AllocateTextBody(f32 textHeight, ui8 flags) {
	FunctionStart(text_body());
	AssertInternal(textHeight > 0);
	
	text_body ret = {};
	ret.memory = AllocateStack();
	ret.textHeight = textHeight;
	ret.flags = flags;
	
	FunctionEnd();
	return ret;
}

dll_export program_external bool TextBodyIsValid(text_body& tb) {
	FunctionStart(false);
	bool ret = IsValid(tb.memory);
	FunctionEnd();
	return ret;
}

dll_export program_external void InsertString(char* string, ui32 length, text_body& tb, ui32 pos) {
	FunctionStart(;);
	
	AssertInternal(string != Null);
	AssertInternal(length > 0);
	AssertInternal(TextBodyIsValid(tb) == true);
	
	if(pos <= tb.memory.size) {
		void* mem = Insert(length, pos, tb.memory);
		CopyMemory((void*)string, length, mem);
	}
	
	FunctionEnd();
}

dll_export program_external void FreeTextBody(text_body& tb) {
	FunctionStart(;);
	if(TextBodyIsValid(tb) == true)
		FreeStack(tb.memory);
	FunctionEnd();
}

dll_export program_external void ClearTextBody(text_body& tb) {
	FunctionStart(;);
	ResetStack(tb.memory);
	FunctionEnd();
}

dll_export program_external ui32 GetTextBodyLength(text_body& tb) {
	FunctionStart(Null);
	AssertInternal(TextBodyIsValid(tb) == true);
	auto ret = tb.memory.size;
	FunctionEnd();
	return ret;
}

dll_export program_external char* GetTextBodyStart(text_body& tb) {
	FunctionStart(Null);
	AssertInternal(TextBodyIsValid(tb) == true);
	auto ret = (char*)tb.memory.memory;
	FunctionEnd();
	return ret;
}

dll_export program_external void RemoveChar(text_body& tb, ui32 pos) {
	FunctionStart(;);
	AssertInternal(TextBodyIsValid(tb) == true);
	if(pos < GetTextBodyLength(tb))
		Remove(sizeof(char), pos, tb.memory);
	FunctionEnd();
}

dll_export program_external vector GetTextRenderDimensions(char* text, ui32 length, f32 height) {
	FunctionStart(vector());
	AssertInternal(text != Null);

	vector ret = CreateVector(Null, height);
	if(length == 0)
		return ret;

	f32 xOffset = 0;
	ForAll(length) {
		if(text[it] == '\n') {
			xOffset = 0;
			ret.y += height * 1.5f;
		}
		else {
			if(xOffset > 0)
				xOffset += height * 0.5f; // Space between glyphs
			xOffset += height;
			ret.x = GetMax(xOffset, ret.x);
		}
	}

	FunctionEnd();
	return ret;
}

program_local void RenderTextLineHor(ui16 x, ui16 y, ui8 height) {
	glVertex2f(x, y);
	glVertex2f(x + height, y);
}

program_local void RenderTextLineVert(ui16 x, ui16 y, ui8 height) {
	glVertex2f(x, y);
	glVertex2f(x, y + height);
}

dll_export program_external rectangle RenderText(char* text, ui32 length, f32 x, f32 y, f32 height, bool center) {
	FunctionStart(rectangle());
	
	AssertInternal(text != Null);
	AssertInternal(length > 0);

	rectangle ret = {};
	ret.left = x;
	ret.bottom = y;

	f32 xOffset = 0;
	if(center == true)
		xOffset = -GetTextRenderDimensions(text, length, height).x / 2;

	f32 nextX = x + xOffset;
	f32 nextY = y;
	glColor3f(1, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	ForAll(length) {
		char c = text[it];
		switch(c) {
			case(' '): break;

			case('\n'): {
				nextY -= height * 1.5f;
				ret.bottom = nextY;
				nextX = x;
			} break;

			case('\b'): {
				RenderTextLineVert(nextX + height / 2, nextY + height / 4, height / 2);
				RenderTextLineHor(nextX + height / 4, nextY + height / 2, height / 2);
			} break;

			case('a'):
			case('A'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
			} break;

			case('b'):
			case('B'):
			case('8'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);

				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('c'):
			case ('C'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('d'):
			case ('D'): {
				RenderTextLineVert(nextX, nextY, height);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height, nextY + height / 2);
				glVertex2f(nextX, nextY);
				glVertex2f(nextX + height, nextY + height / 2);
			} break;

			case ('e'):
			case ('E'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('f'):
			case ('F'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
			} break;

			case ('g'):
			case ('G'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY, height);
				glVertex2f(nextX + height, nextY);
				glVertex2f(nextX + height, nextY + height / 2);
			} break;

			case ('h'):
			case ('H'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
			} break;

			case ('i'):
			case ('I'):
			case ('1'): {
				RenderTextLineVert(nextX + height / 2, nextY, height);
			} break;

			case ('j'):
			case ('J'): {
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
			} break;

			case ('k'):
			case ('K'): {
				RenderTextLineVert(nextX, nextY, height);
				glVertex2f(nextX, nextY + height / 2);
				glVertex2f(nextX + height, nextY + height);
				glVertex2f(nextX, nextY + height / 2);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('l'):
			case ('L'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('m'):
			case ('M'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height, nextY + height);
			} break;

			case ('n'):
			case ('N'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('o'):
			case ('O'):
			case ('0'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('p'):
			case ('P'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				glVertex2f(nextX + height, nextY + height);
				glVertex2f(nextX + height, nextY + height / 2);
			} break;

			case ('q'):
			case ('Q'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height / 2, nextY, height);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height / 2, nextY + height);
				glVertex2f(nextX, nextY);
				glVertex2f(nextX + height / 2, nextY);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('r'):
			case ('R'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				glVertex2f(nextX + height, nextY + height);
				glVertex2f(nextX + height, nextY + height / 2);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('s'):
			case ('S'):
			case ('5'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineHor(nextX, nextY, height);
				glVertex2f(nextX, nextY + height / 2);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height, nextY);
				glVertex2f(nextX + height, nextY + height / 2);
			} break;

			case ('t'):
			case ('T'): {
				RenderTextLineVert(nextX + height / 2, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
			} break;

			case ('u'):
			case ('U'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('v'):
			case ('V'): {
				glVertex2f(nextX + height / 2, nextY);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height / 2, nextY);
				glVertex2f(nextX + height, nextY + height);
			} break;

			case ('w'):
			case ('W'): {
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height);
				glVertex2f(nextX, nextY);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('x'):
			case ('X'): {
				glVertex2f(nextX, nextY);
				glVertex2f(nextX + height, nextY + height);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height, nextY);
			} break;

			case ('y'):
			case ('Y'): {
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX + height, nextY + height);
				glVertex2f(nextX + height / 2, nextY + height / 2);
				glVertex2f(nextX, nextY + height);
				glVertex2f(nextX + height / 2, nextY);
				glVertex2f(nextX + height / 2, nextY + height / 2);
			} break;

			case ('z'):
			case ('Z'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY, height);
				glVertex2f(nextX, nextY);
				glVertex2f(nextX + height, nextY + height);
			} break;

			case ('2'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineVert(nextX + height, nextY + height / 2, height / 2);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineVert(nextX, nextY, height / 2);
				RenderTextLineHor(nextX, nextY, height);

			} break;

			case ('3'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineVert(nextX + height, nextY + height / 2, height / 2);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineVert(nextX + height, nextY, height / 2);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			case ('4'): {
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineVert(nextX + height, nextY, height);
				glVertex2f(nextX, nextY + height / 2);
				glVertex2f(nextX + height, nextY + height);
			} break;

			case ('6'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineVert(nextX, nextY, height);
				RenderTextLineHor(nextX, nextY, height);
				RenderTextLineVert(nextX + height, nextY, height / 2);
				RenderTextLineHor(nextX, nextY + height / 2, height);
			} break;

			case ('7'): {
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineVert(nextX + height, nextY, height);
			} break;

			case ('9'): {
				RenderTextLineVert(nextX, nextY + height / 2, height / 2);
				RenderTextLineVert(nextX + height, nextY, height);
				RenderTextLineHor(nextX, nextY + height, height);
				RenderTextLineHor(nextX, nextY + height / 2, height);
				RenderTextLineHor(nextX, nextY, height);
			} break;

			default: break;
		}

		ret.width = GetMax(ret.width, nextX + height - ret.left);

		if(c != '\n')
			nextX += height * 1.5f;
	}
	glEnd();
	AssertOpenGL();

	ret.height = y + height - ret.bottom;
	AssertInternal(ret.width != 0);
	AssertInternal(ret.height != 0);

	FunctionEnd();
	return ret;
}
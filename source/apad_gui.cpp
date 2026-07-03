#include <windows.h>
#include <gl\gl.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_gui.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_opengl.h"
#include "apad_opengl_internal.h"
#include "apad_string.h"

program_local 			text_body* CurrentTextBody;
program_local 			ui16       CursorCharOffset; // 0-based from the start of CurrentTextBody text
program_local 			vector     CursorPos;
program_local 			f32        CursorBlinkTimeElapsed;
program_local const f32  			 CursorBlinkFullLength = 1.5f; // Time to go fully transparent and back to full opaqueness

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

dll_export program_external ui16 InsertString(char* string, ui32 length, text_body& tb, ui32 pos) {
	FunctionStart(Null);
	
	AssertInternal(string != Null);
	AssertInternal(length > 0);
	AssertInternal(TextBodyIsValid(tb) == true);
	
	if(pos > GetTextBodyLength(tb))
		return Null;
	
	ui16 added = 0;
	ForAll(length) {
		char c = string[it];
		
		bool add = true;
		if(IsLetter(c) == true && BitIsSet(TextBodyFlagLetters, tb.flags) == false ||
			c == BulletPointChar && BitIsSet(TextBodyFlagBulletPoints, tb.flags) == false ||
			c == NewlineChar && BitIsSet(TextBodyFlagNewlines, tb.flags) == false)
			add = false;
		
		if(add == true) {
			void* mem = Insert(1, pos + added, tb.memory);
			*((char*)mem) = c;
			added += 1;
		}
	}
	
	FunctionEnd();
	return added;
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

dll_export program_external char* GetTextBodyText(text_body& tb) {
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

dll_export program_external void RemoveCharBeforeCursor() {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);
	bool del = CursorCharOffset > 0; // If the cursor was already at 0, moving down would incorrectly deleted the very first letter
	MoveCursor(-1);
	if(del == true)
		RemoveChar(*CurrentTextBody, CursorCharOffset);
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
		if(text[it] == NewlineChar) {
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

			case(NewlineChar): {
				nextY -= height * 1.5f;
				ret.bottom = nextY;
				nextX = x;
			} break;

			case(BulletPointChar): {
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

		if(c != NewlineChar)
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

dll_export program_external void SetCursorPos(f32 x, f32 y) {
	CursorPos.x = x;
	CursorPos.y = y;
}

dll_export program_external bool TextIsBeingUpdated() {
	return CurrentTextBody != Null;
}

dll_export program_external void MoveCursor(si8 charOffset) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);

	auto textLength = GetTextBodyLength(*CurrentTextBody);
	AssertInternal(CursorCharOffset <= textLength);

	if(charOffset < 0) {
		if(-charOffset >= CursorCharOffset)
			CursorCharOffset = 0;
		else
			CursorCharOffset += charOffset;
	}
	else if(charOffset > 0) {
		if(CursorCharOffset + charOffset >= textLength)
			CursorCharOffset = textLength;
		else
			CursorCharOffset += charOffset;
	}

	CursorBlinkTimeElapsed = 0;
	FunctionEnd();
}

dll_export program_external void InsertCharAtCursor(char c) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);
	
	auto inserted = InsertString(&c, 1, *CurrentTextBody, CursorCharOffset);
	CursorCharOffset += inserted;
	
	FunctionEnd();
}

dll_export program_external ui16 GetCharOffsetFromStart(char* c) {
	FunctionStart(Null);
	AssertInternal(TextIsBeingUpdated() == true);
	auto ret = (ui16)((ui8*)c - (ui8*)GetTextBodyText(*CurrentTextBody));
	FunctionEnd();
	return ret;
}

dll_export program_external char* FindChar(char c, ui16 pos, bool scanForward) {
	FunctionStart(Null);
	AssertInternal(TextIsBeingUpdated() == true);

	if(scanForward == false && pos == 0)
		return Null;

	char* text = GetTextBodyText(*CurrentTextBody);
	ui32  start = scanForward == true ? pos : pos - 1;
	ui32  end = scanForward == true ? GetTextBodyLength(*CurrentTextBody) : 0;
	FromTo(start, end) {
		if(text[it] == c)
			return text + it;
	}

	FunctionEnd();
	return Null;
}

dll_export program_external void EndTextUpdate() {
	CurrentTextBody = Null;
	CursorBlinkTimeElapsed = 0;
	ClearStruct(CursorPos);
	CursorCharOffset = 0;
}


dll_export program_external void BeginTextUpdate(text_body& text) {
	CurrentTextBody = &text;
	CursorCharOffset = GetTextBodyLength(text);
	CursorBlinkTimeElapsed = 0;
}

dll_export program_external void DrawRectangleBorder(f32 left, f32 bottom, f32 width, f32 height, f32 lineWidth, ui8 r, ui8 g, ui8 b) {
	FunctionStart(;);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glColor3f(UI8ColourToF32(r), UI8ColourToF32(g), UI8ColourToF32(b));

	glVertex2f(left, bottom);
	glVertex2f(left, bottom + height);

	glVertex2f(left, bottom + height);
	glVertex2f(left + width, bottom + height);

	glVertex2f(left + width, bottom + height);
	glVertex2f(left + width, bottom);

	glVertex2f(left + width, bottom);
	glVertex2f(left, bottom);
	glEnd();
	AssertInternalGL();
	FunctionEnd();
}

dll_export program_external void DrawCircleBorder(f32 centerX, f32 centerY, f32 radius, ui8 lineWidth, ui8 r, ui8 g, ui8 b) {
	FunctionStart(;);
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	glColor3f(UI8ColourToF32(r), UI8ColourToF32(g), UI8ColourToF32(b));
	ui8 vertices = 72;
	FromToInc(0, vertices + 1) {
		f32 angle = it * 360 / vertices;
		f32 x = centerX - Sine(angle) * radius;
		f32 y = centerY + Cos(angle) * radius;
		glVertex2f(x, y);
	}
	glEnd();
	FunctionEnd();
}

dll_export program_external void DrawRectangleFull(f32 left, f32 bottom, f32 width, f32 height, ui8 r, ui8 g, ui8 b) {
	FunctionStart(;);
	glBegin(GL_QUADS);
	glColor3f(UI8ColourToF32(r), UI8ColourToF32(g), UI8ColourToF32(b));
	glVertex2f(left, bottom);
	glVertex2f(left + width, bottom);
	glVertex2f(left + width, bottom + height);
	glVertex2f(left, bottom + height);
	glEnd();
	AssertInternalGL();
	FunctionEnd();
}

dll_export program_external f32 UI8ColourToF32(ui8 u) {
	return (f32)u / 255;
}
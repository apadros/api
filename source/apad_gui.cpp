#include <windows.h>
#include <gl\gl.h>
#include <stdarg.h> // For varargs
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_gui.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"
#include "apad_opengl.h"
#include "apad_opengl_internal.h"
#include "apad_string.h"
#include "apad_win32_gui.h"

program_local 			text_body* CurrentTextBody;
program_local 			ui16       CursorCharOffset; // 0-based from the start of CurrentTextBody text
program_local 			vector     CursorPos; // Relative to the bottom-left corner of CurrentTextBody
program_local 			f32        CursorBlinkTimeElapsed;
program_local const f32  			 CursorBlinkFullLength = 1.5f; // Time to go fully transparent and back to full opaqueness
program_local       f32        CursorAlpha;

// @TODO - This will need to be updated when introducing fonts
dll_export program_external f32 GetTextLineHeight(f32 textHeight) {
	return textHeight * 1.5f;
}

// @TODO - This will need to be updated when introducing fonts
program_local f32 GetGlyphWidth(f32 textHeight) {
	return textHeight;
}

// @TODO - This will need to be updated when introducing fonts
program_local f32 GetGlyphSpaceWidth(f32 textHeight) {
	return textHeight * 0.5f;
}

struct text_body_line {
	char* start;
	char* end; // Not inclusive, will point to newline char if present at the end of the line
	ui16  charLength; // Doesn't count newline chars
};
program_local text_body_line GetTextBodyLine(ui16 charOffset, text_body& tb) {
	FunctionStart(text_body_line());

	char* start = FindChar(NewlineChar, charOffset, false, tb);
	if(start == Null) // We're at the first line
		start = GetText(tb);
	else
		start += 1; // Remove the newline char
	
	char* end = FindChar(NewlineChar, charOffset, true, tb);
	if(end == Null) // We're at the last line
		end = GetText(tb) + GetTextLength(tb);
	AssertInternal(end >= start);

	text_body_line ret = {};
	ret.start = start;
	ret.end = end;
	ret.charLength = (ui8*)end - (ui8*)start;

	FunctionEnd();
	return ret;
}

dll_export program_external text_body AllocateTextBody(f32 left, f32 bottom, f32 width, f32 textBorderOffset, f32 textHeight, ui8 flags) {
	FunctionStart(text_body());
	AssertInternal(width > 0);
	AssertInternal(textBorderOffset > 0);
	AssertInternal(textHeight > 0);

	text_body ret = {};
	ret.memory = AllocateStack();
	ret.container = CreateRectangle(left, bottom, width, textHeight + textBorderOffset * 2);
	ret.textBorderOffset = textBorderOffset;
	ret.textHeight = textHeight;
	ret.flags = flags;

	FunctionEnd();
	return ret;
}

dll_export program_external bool IsValid(text_body& tb) {
	FunctionStart(false);
	bool ret = IsValid(tb.memory);
	FunctionEnd();
	return ret;
}

dll_export program_external ui16 Insert(char* string, ui32 length, text_body& tb, ui32 pos) {
	FunctionStart(Null);

	AssertInternal(string != Null);
	AssertInternal(length > 0);
	AssertInternal(IsValid(tb) == true);

	if(pos > GetTextLength(tb))
		return Null;

	ui16 added = 0;
	ForAll(length) {
		char c = string[it];

		bool add = true;
		if(IsLetter(c) == true && (tb.flags & TextBodyFlagLetters) == 0 ||
			c == BulletPointChar && (tb.flags & TextBodyFlagBulletPoints) == 0 ||
			c == NewlineChar && (tb.flags & TextBodyFlagNewlines) == 0)
			add = false;

		// If wanting to add a bullet point, check if previous char is already one and, if so, don't add
		if(add == true && c == BulletPointChar && pos > 0 && GetText(tb)[pos - 1] == BulletPointChar)
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

dll_export program_external void FreeText(text_body& tb) {
	FunctionStart(;);
	if(IsValid(tb) == true)
		Free(tb.memory);
	FunctionEnd();
}

dll_export program_external void ClearText(text_body& tb) {
	FunctionStart(;);
	Reset(tb.memory);
	FunctionEnd();
}

dll_export program_external ui32 GetTextLength(text_body& tb) {
	FunctionStart(Null);
	AssertInternal(IsValid(tb) == true);
	auto ret = tb.memory.size;
	FunctionEnd();
	return ret;
}

dll_export program_external char* GetText(text_body& tb) {
	FunctionStart(Null);
	AssertInternal(IsValid(tb) == true);
	auto ret = (char*)tb.memory.memory;
	FunctionEnd();
	return ret;
}

dll_export program_external void InsertCharAtCursor(char c) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);

	auto inserted = Insert(&c, 1, *CurrentTextBody, CursorCharOffset);
	CursorCharOffset += inserted;

	FunctionEnd();
}

dll_export program_external ui16 GetCursorCharOffset() {
	return CursorCharOffset;
}

dll_export program_external text_update_pipeline_data RunTextUpdatePipeline(win32_state& osState) {
	FunctionStart(text_update_pipeline_data());
	AssertInternal(TextIsBeingUpdated() == true);

	text_update_pipeline_data ret = {};

	if(osState.keyPressed != Null) // Add text
		InsertCharAtCursor(osState.keyPressed);
	else if(osState.backspacePressed == true){ // Remove text
		bool del = CursorCharOffset > 0; // If the cursor was already at 0, moving down would incorrectly deleted the very first letter
		MoveCursor(-1);
		if(del == true)
			RemoveChar(*CurrentTextBody, CursorCharOffset);
	}
	else if(osState.enterPressed == true) { // Jump to next line if allowed, otherwise end writing
	  if(CurrentTextBody->flags & TextBodyFlagNewlines > 0) {
			// Scan back to see if the current line contains a bullet point
			bool  bulletPoint = false;
			char* text = GetText(*CurrentTextBody);
			FromTo(CursorCharOffset, 0) {
				char c = text[it];
				if(c == BulletPointChar) {
					bulletPoint = true;
					break;
				}
				else if(c == NewlineChar)
					break;
			}

			InsertCharAtCursor(NewlineChar);

			if(bulletPoint == true)
				InsertCharAtCursor(BulletPointChar);
		}
		else
			EndTextUpdate();
	}
	else if(CursorCharOffset >= 1 && GetText(*CurrentTextBody)[CursorCharOffset - 1] == BulletPointChar && osState.tabPressed == true) // Remove bullet point if tab is pressed after it
		GetText(*CurrentTextBody)[CursorCharOffset - 1] = ' ';
	else if(osState.escapePressed == true) // Esc hit
		EndTextUpdate();
	else if(osState.leftPressed == true && CursorCharOffset >= 1)
		MoveCursor(-1);
	else if(osState.rightPressed == true && CursorCharOffset < GetTextLength(*CurrentTextBody))
		MoveCursor(1);
	else if(osState.downPressed == true) { // Move down one line within text body if possible
		// Need to scan behind and in front of the cursor to determine the bounds of the current line
		char* end = FindChar(NewlineChar, CursorCharOffset, true, *CurrentTextBody);
		if(end != Null) {
			ui32  lineStartOffset = CursorCharOffset;
			char* start = FindChar(NewlineChar, CursorCharOffset, false, *CurrentTextBody);
			if(start != Null)
				lineStartOffset -= (ui8*)start + 1 - (ui8*)GetText(*CurrentTextBody);

			ui32 delta = (ui32)((ui8*)end + 1 - CursorCharOffset + lineStartOffset);
			MoveCursor(delta);
		}
		else
			ret.wantToLeaveTextBodyDown = true;
	}
	else if(osState.upPressed == true) {
		char* start = FindChar(NewlineChar, CursorCharOffset, false, *CurrentTextBody);
		if(start != Null) { // Move up one line within text body
			// Need to scan behind and in front of the cursor to determine the bounds of the current line
			bool  previousLineIsLonger = false;
			char* previousLineStart = FindChar(NewlineChar, GetCharOffsetFromStart(start), false, *CurrentTextBody);
			if(previousLineStart == Null) // The line above is the very first one
				previousLineIsLonger = GetCharOffsetFromStart(start) > CursorCharOffset - GetCharOffsetFromStart(start + 1);
			else { // The line above is at least the second in the paragraph
				auto lineLength = GetCharOffsetFromStart(start) - GetCharOffsetFromStart(previousLineStart + 1);
				previousLineIsLonger = lineLength > CursorCharOffset - GetCharOffsetFromStart(start + 1);
			}

			if(previousLineIsLonger == true) { // Just move cursor up
				ui16 cursorCharOffset = CursorCharOffset - GetCharOffsetFromStart(start + 1);
				ui16 lineStartIndex = previousLineStart == Null ? 0 : GetCharOffsetFromStart(previousLineStart + 1);
				CursorCharOffset = lineStartIndex + cursorCharOffset;
			}
			else // Place cursor at the end of the previous line
				MoveCursor(GetCharOffsetFromStart(start) - CursorCharOffset);
		}
		else
			ret.wantToLeaveTextBodyUp = true;
	}

	// Update cursor pos relative to text body bottom-left corner
	if(TextIsBeingUpdated() == true) { // In case esc is hit before this point
		vector pos = { 0, -CurrentTextBody->textHeight };
		AssertInternal(CursorCharOffset <= GetTextLength(*CurrentTextBody));
		auto* text = GetText(*CurrentTextBody);
		ForAll(CursorCharOffset) {
			if(text[it] == NewlineChar) {
				pos.x = 0;
				pos.y -= GetTextLineHeight(CurrentTextBody->textHeight);
			}
			else
				pos.x += GetGlyphWidth(CurrentTextBody->textHeight) + GetGlyphSpaceWidth(CurrentTextBody->textHeight);
		}
		if(CursorCharOffset > 0)
			pos.x -= CurrentTextBody->textHeight * 0.25f; // Place half way between 2 glyphs

		f32 textBodyHeight = GetTextRectangle(*CurrentTextBody).height;
		pos.y = textBodyHeight + pos.y;
		CursorPos = pos;
	}

	// Update cursor blink animation timeline
	CursorBlinkTimeElapsed += osState.lastFrameTime;
	if(CursorBlinkTimeElapsed > CursorBlinkFullLength) {
		// In case we get a frame time >= CursorBlinkFullLength * 2 for whatever reason
		do 		CursorBlinkTimeElapsed -= CursorBlinkFullLength;
		while(CursorBlinkTimeElapsed > CursorBlinkFullLength);
	}
	AssertInternal(CursorBlinkTimeElapsed >= 0);
	AssertInternal(CursorBlinkTimeElapsed <= CursorBlinkFullLength);
	
	// Update alpha value
	if(CursorBlinkTimeElapsed >= 0 && CursorBlinkTimeElapsed < CursorBlinkFullLength / 2)
		CursorAlpha = LERP(1.0f, 0.0f, CursorBlinkTimeElapsed / (CursorBlinkFullLength / 2));
	else
		CursorAlpha = LERP(0.0f, 1.0f, (CursorBlinkTimeElapsed - CursorBlinkFullLength / 2) / (CursorBlinkFullLength / 2)); 
	AssertInternal(CursorAlpha >= 0);
	AssertInternal(CursorAlpha <= 1.0f);
			

	FunctionEnd();
	return ret;
}

dll_export program_external void RemoveChar(text_body& tb, ui32 pos) {
	FunctionStart(;);
	AssertInternal(IsValid(tb) == true);
	if(pos < GetTextLength(tb))
		Remove(sizeof(char), pos, tb.memory);
	FunctionEnd();
}

dll_export program_external rectangle GetTextRectangle(text_body& tb) {
	FunctionStart(rectangle());
	
	vector size = CreateVector(Null, tb.textHeight);
	auto length = GetTextLength(tb);
	if(length > 0)
		size = GetTextRenderSize(GetText(tb), length, tb.textHeight);
	
	f32 left = Null;
	if((tb.flags & TextBodyFlagLeftAligned) > 0)
		left = tb.container.left + tb.textBorderOffset;
	else // Horizontally centered
		left = GetCenter(tb.container).x - size.x / 2;
	
	f32 bottom = tb.container.bottom + tb.container.height - tb.textBorderOffset - size.y;
	
	// Avoid using CreateRectangle() since it won't allow width == 0
	rectangle ret = {};
	ret.pos = CreateVector(left, bottom);
	ret.size = size;
	
	FunctionEnd();
	return ret;
}

dll_export program_external vector GetTextRenderSize(char* text, ui32 length, f32 height) {
	FunctionStart(vector());
	AssertInternal(text != Null);
	AssertInternal(height > 0);

	vector ret = CreateVector(Null, height);

	f32 xOffset = 0;
	if(length == 0)
		length = GetLength(text);
	ForAll(length) {
		if(text[it] == NewlineChar) {
			xOffset = 0;
			ret.y += GetTextLineHeight(height);
		}
		else {
			if(xOffset > 0)
				xOffset += GetGlyphSpaceWidth(height);
			xOffset += GetGlyphWidth(height);
			ret.x = GetMax(xOffset, ret.x);
		}
	}

	FunctionEnd();
	return ret;
}

dll_export program_external void Render(text_body& tb) {
	auto length = GetTextLength(tb);
	if(length > 0) {
		if((tb.flags & TextBodyFlagLeftAligned) > 0)
			RenderText(GetText(tb), length, tb.container.left + tb.textBorderOffset, GetTopRight(tb.container).y - tb.textBorderOffset - tb.textHeight, tb.textHeight, false);
		else
			RenderText(GetText(tb), length, GetCenter(tb.container).x, GetTopRight(tb.container).y - tb.textBorderOffset - tb.textHeight, tb.textHeight, true);
	}
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
	
	if(length == 0)
		length = GetLength(text);

	rectangle ret = {};
	ret.left = x;
	ret.bottom = y;

	f32 xOffset = 0;
	if(center == true)
		xOffset = -GetTextRenderSize(text, length, height).x / 2;

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
				nextY -= GetTextLineHeight(height);
				ret.bottom = nextY;
				nextX = x;
			} break;

			case(BulletPointChar): {
				RenderTextLineVert(nextX + height / 2, nextY + height / 4, height / 2);
				RenderTextLineHor(nextX + height / 4, nextY + height / 2, height / 2);
			} break;
			
			case('#'): {
				RenderTextLineHor(nextX, nextY + height / 3, height);
				RenderTextLineHor(nextX, nextY + height * 2 / 3, height);
				RenderTextLineVert(nextX + height / 3, nextY, height);
				RenderTextLineVert(nextX + height * 2 / 3, nextY, height);
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
			nextX += GetGlyphWidth(height) + GetGlyphSpaceWidth(height);
	}
	glEnd();
	AssertOpenGL();

	ret.height = y + height - ret.bottom;
	AssertInternal(ret.width != 0);
	AssertInternal(ret.height != 0);

	FunctionEnd();
	return ret;
}

dll_export program_external vector GetCursorPos() {
	return CursorPos;
}

dll_export program_external bool TextIsBeingUpdated() {
	return CurrentTextBody != Null;
}

dll_export program_external void MoveCursor(si8 charOffset) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);

	si32 finalOffset = (si32)CursorCharOffset + charOffset;
	if(finalOffset < 0)
		finalOffset = 0;
	SetCursorCharOffset(finalOffset); // Will clamp to text_body length

	FunctionEnd();
}

dll_export program_external void _SetCursorPos(f32 x, f32 y) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);
	auto size = GetTextRectangle(*CurrentTextBody).size;
	Clamp(x, 0, size.width);
	Clamp(y, 0, size.height);

	ui16 lineNumber = (size.height - y) / GetTextLineHeight(CurrentTextBody->textHeight); // 0-based

	// Get the char offset to correct line first
	ui16 charOffset = 0;
	ForAll(lineNumber) {
		auto line = GetTextBodyLine(charOffset, *CurrentTextBody);
		charOffset += line.charLength;
		if(lineNumber > 0)
			charOffset += 1; // Newline char
	}

	// Then check where target x is in current line
	{
		auto line = GetTextBodyLine(charOffset, *CurrentTextBody);
		f32  glyphWidth = GetGlyphWidth(CurrentTextBody->textHeight);
		f32  glyphSpace = GetGlyphSpaceWidth(CurrentTextBody->textHeight);
		f32  pixelLength = (glyphWidth + glyphSpace) * line.charLength - glyphSpace;
		if(pixelLength < 0) // This can happen if the target line is the very last and is empty (i.e. enter was just hit at the end of the text body)
			pixelLength = 0;
		if(x == 0)
			SetCursorCharOffset(charOffset);
		else if(x >= pixelLength)
			SetCursorCharOffset(charOffset + line.charLength);
		else {
			ui16 offset = x / (glyphWidth + glyphSpace); // Not completely accurate but close enough for now
			SetCursorCharOffset(charOffset + offset + 1); // Place to the right of selected glyph
		}
	}

	FunctionEnd();
}

dll_export program_external ui16 GetCharOffsetFromStart(char* c) {
	FunctionStart(Null);
	AssertInternal(TextIsBeingUpdated() == true);
	auto ret = (ui16)((ui8*)c - (ui8*)GetText(*CurrentTextBody));
	FunctionEnd();
	return ret;
}

dll_export program_external char* FindChar(char c, ui16 pos, bool scanForward, text_body& tb) {
	if(scanForward == false && pos == 0)
		return Null;

	FunctionStart(Null);
	AssertInternal(TextIsBeingUpdated() == true);

	char* text = GetText(tb);
	ui32  start = scanForward == true ? pos : pos - 1;
	ui32  end = scanForward == true ? GetTextLength(tb) : 0;
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
	ClearInstance(CursorPos);
	CursorCharOffset = 0;
}

dll_export program_external text_body* GetCurrentTextBody() {
	FunctionStart(Null);
	AssertInternal(TextIsBeingUpdated() == true);
	FunctionEnd();
	return CurrentTextBody;
}

dll_export void SetCursorCharOffset(ui16 offset) {
	FunctionStart(;);
	AssertInternal(TextIsBeingUpdated() == true);

	auto length = GetTextLength(*CurrentTextBody);
	if(offset > length)
		offset = length;
	CursorCharOffset = offset;
	CursorBlinkTimeElapsed = 0;

	FunctionEnd();
}

dll_export program_external void BeginTextUpdate(text_body& text) {
	CurrentTextBody = &text;
	CursorCharOffset = GetTextLength(text);
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
	AssertInternalGL();
	FunctionEnd();
}

dll_export program_external void DrawCircleFull(f32 centerX, f32 centerY, f32 radius, ui8 r, ui8 g, ui8 b, f32 a) {
	FunctionStart(;);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(UI8ColourToF32(r), UI8ColourToF32(g), UI8ColourToF32(b), a);
	ui8 vertices = 72;
	FromToInc(0, vertices + 1) {
		f32 angle = it * 360 / vertices;
		f32 x = centerX - Sine(angle) * radius;
		f32 y = centerY + Cos(angle) * radius;
		glVertex2f(x, y);
	}
	glEnd();
	AssertInternalGL();
	FunctionEnd();
}

dll_export program_external void DrawRectangleFull(f32 left, f32 bottom, f32 width, f32 height, ui8 r, ui8 g, ui8 b, f32 a) {
	FunctionStart(;);
	glBegin(GL_QUADS);
	glColor4f(UI8ColourToF32(r), UI8ColourToF32(g), UI8ColourToF32(b), a);
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

dll_export program_external f32 GetCursorAlphaValue() {
	return CursorAlpha;
}

dll_export program_external button AllocateButton(f32 left, f32 bottom, f32 width, f32 height, char* text, f32 textHeight, ui8 highlightRed, ui8 highlightGreen, ui8 highlightBlue, f32 highlightAlpha) {
	FunctionStart(button());
	AssertInternal(text != Null);
	AssertInternal(textHeight != Null);
	
	button ret;
	ret.rectangle = CreateRectangle(left, bottom, width, height);
	ret.text = AllocateString(text);
	ret.textHeight = textHeight;
	ret.highlightColour = CreateColour(highlightRed, highlightGreen, highlightBlue);
	ret.highlightAlpha = highlightAlpha;
	
	FunctionEnd();
	return ret;
}

dll_export program_external void FreeButtonText(button& b) {
	FunctionStart(;);
	Free(b.text);
	FunctionEnd();
}

dll_export program_external bool ButtonClicked(button& b, win32_state& state) {
	return Win32MouseLeftClickedThisFrame(state) == true && Overlap(state.mouseX, state.mouseY, UnpackRectangle(b.rectangle)) == true;
}

dll_export program_external void Render(button& b, f32 mouseX, f32 mouseY) {
	FunctionStart(;);
	if(b.highlightAlpha > 0 && Overlap(mouseX, mouseY, UnpackRectangle(b.rectangle)) == true)
		DrawRectangleFull(UnpackRectangle(b.rectangle), UnpackColourUI8(b.highlightColour), b.highlightAlpha);
	RenderText(b.text, Null, GetCenter(b.rectangle).x, GetCenter(b.rectangle).y - b.textHeight / 2, b.textHeight, true);
	FunctionEnd();
}

dll_export program_external colour CreateColour(ui8 r, ui8 g, ui8 b) {
	colour ret;
	ret.red = (f32)r / 255;
	ret.green = (f32)g / 255;
	ret.blue = (f32)b / 255;
	return ret;
}

dll_export program_external ui_element_layout* GetUIElementLayouts(f32 start, f32 end, ui8 count, ...) {
	FunctionStart(Null);
	AssertInternal(count > 0);
	
	ui_element_layout* layouts = (ui_element_layout*)Win32AllocateMemory(sizeof(ui_element_layout) * count);
	AssertInternal(layouts != Null);
	
	va_list list;
	va_start(list, count);
	
	f32 totalSize = 0;
	ForAll(count) {
		f32 size = va_arg(list, f64);
		layouts[it].size = size;
		totalSize += size;
	}
	
	f32 range = end - start;
	AssertInternal(Magnitude(totalSize) <= Magnitude(range));
	
	AssertInternal(count >= 1);
	f32 offset = (range - totalSize) / (count - 1);
	
	f32 nextStart = start;
	ForAll(count) {
		auto* l = layouts + it;
		l->start = nextStart;
		l->end = l->start + l->size;
		l->center = l->start + l->size / 2;
		nextStart += l->size + offset;
	}
	
	va_end(list);
	
	FunctionEnd();
	return layouts;
}

dll_export program_external void FreeUIElementLayouts(ui_element_layout* layouts) {
		FunctionStart(;);
		AssertInternal(layouts != Null);
		Win32FreeMemory((void*)layouts);
		FunctionEnd();
}
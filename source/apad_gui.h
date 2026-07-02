#ifndef APAD_GUI_H
#define APAD_GUI_H

#include "apad_base_types.h"
#include "apad_memory.h"

// ******************** Text body ******************** //

const ui8 TextBodyFlagLetters = 		 1;
const ui8 TextBodyFlagBulletPoints = 1 << 1;
const ui8 TextBodyFlagTabs = 				 1 << 2;
const ui8 TextBodyFlagNewlines = 		 1 << 3;
const ui8 TextBodyFlagLeftAligned =  1 << 4; // If not present text is assumed to be center-aligned

struct text_body {
	memory_stack memory;
	f32          textHeight;
	ui8          flags;
};

dll_import text_body AllocateTextBody(f32 textHeight, ui8 flags);
dll_import void 		 ClearTextBody(text_body& tb);
dll_import void 		 FreeTextBody(text_body& tb);
dll_import ui32 		 GetTextBodyLength(text_body& tb);
dll_export char* 		 GetTextBodyStart(text_body& tb);
dll_import void 		 InsertString(char* string, ui32 length, text_body& tb, ui32 pos);
dll_import void 		 RemoveChar(text_body& tb, ui32 pos);
dll_import bool 		 TextBodyIsValid(text_body& tb);

// ******************** Others ******************** //

dll_export vector    GetTextRenderDimensions( // Will return a minimum y of height even if no text present, but x will equal 0
									   												  char* text, 
									   												  ui32  length, 
									   												  f32   height); 
dll_export rectangle RenderText(char* text, 
																ui32  length, 
																f32 	x, 
																f32 	y, 
																f32 	height, 
																bool  center);

#endif
#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h> // For varargs
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"
#include "apad_log.h"
#include "apad_string.h"
#include "apad_string_internal.h"

// ******************** Internal API start ******************** //

// ******************** Internal API end ******************** //

dll_export log_file OpenLogFile() {
	FunctionStart(log_file());
	
	auto ret = AllocateStack(KiB(10));
	
	FunctionEnd();
  return ret;
}

dll_export void CloseLogFile(log_file& log) {
	FunctionStart(;);
	
	Free(log);
	
	FunctionEnd();
}

// Format strings - s, si8, ui8, si16, ui16, si32, ui32, si64, ui64, f32, f64
dll_export void Log(log_file& log, string_args(formatString), ...) {
	FunctionStart(;);
	CheckStringInternal(UnpackStringArgs(formatString));
	
	va_list args;
	va_start(args, formatString);
	
	ForAll(stringArgsLength(formatString)) {
    char c = formatString[it];
		if(c == '%') { // Formatting char
		  it += 1;
			if(formatString[it] == '\0') {
				FunctionEnd();
				return;
			}
				
			char* formats[] = { "s", "si8", "ui8", "si16", "ui16", "si32", "ui32", "si64", "ui64", "f32", "f64" };
			
			// Find the correct format
			si8 formatIndex = -1;
			{
				ui8  formatsLength = GetArrayLength(formats);
				auto sub = ExtractSubstring(formatString + it, 4);
				
				ForAll(formatsLength) {
					if(AreEqual(sub, Null, formats[it], Null) == true) { // Found the format
						formatIndex = it;
						break;
					}
				}
				
				if(formatIndex == -1) {
					sub[3] = '\0';
					
					ForAll(formatsLength) {
						if(AreEqual(sub, Null, formats[it], Null) == true) { // Found the format
							formatIndex = it;
							break;
						}
					}
					
					if(formatIndex == -1) { // Last attempt, compare to string format
						sub[1] = '\0';
						if(AreEqual(sub, Null, formats[0], Null) == true) 
							formatIndex = 0;
					}
				}
			}
			AssertInternal(formatIndex != -1); // @TODO - Do we want to exit more gracefully or continue execution for the rest of the format string?
			
			// Process argument
			switch(formatIndex) {
				case 0: { // String
				  auto s = va_arg(args, char*);
					Push(s, Null, false, log);
					it += 1;
				} break;
				
				case 1: { // SI8
					si8 i = va_arg(args, si8);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 3;
				} break;
				
				case 2: { // UI8
					ui8 i = va_arg(args, ui8);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 3;
				} break;
				
				case 3: { // SI16
					si16 i = va_arg(args, si16);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 4: { // UI16
					ui16 i = va_arg(args, ui16);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 5: { // SI32
					si32 i = va_arg(args, si32);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 6: { // UI32
					ui32 i = va_arg(args, ui32);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 7: { // SI64
					si64 i = va_arg(args, si64);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 8: { // UI64
					ui64 i = va_arg(args, ui64);
					char* s = ToString(i);
					Push(s, Null, false, log);
					it += 4;
				} break;
				
				case 9: { // F32
					f32 f = (f32)va_arg(args, f64);
					char* s = ToString(f);
					Push(s, Null, false, log);
					it += 3;
				} break;
				
				case 10: { // F64
				  f64 f = va_arg(args, f64);
					char* s = ToString(f);
					Push(s, Null, false, log);
					it += 3;
				} break;
				
				default: break;
			};		
			
			it -= 1; // To compensate the +1 in the loop header
		}
		else
			Push((void*)&c, sizeof(c), log);
  }
	
	va_end(args);
	
	FunctionEnd();
}
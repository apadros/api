#ifndef APAD_STRING_H
#define APAD_STRING_H

#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_memory.h"

// ******************** Conversions ******************** //

dll_import char* // Contains a null char at the end. Must be freed with Free(char*) 
								 ConvertStringToLowerCase(const char* s, 
																								ui16  length); // Set to Null to scan until the EOS char

// All ToString() functions return a string allocated on global API memory.
dll_import char* ToString(si8 i);
dll_import char* ToString(ui8 i);
dll_import char* ToString(si16 i);
dll_import char* ToString(ui16 i);
dll_import char* ToString(si32 i);
dll_import char* ToString(ui32 i);
dll_import char* ToString(si64 i);
dll_import char* ToString(ui64 i);
dll_import char* ToString(f32 f); // Return limited to 2 decimal places with rounding
dll_import char* ToString(f64 f); // Return limited to 2 decimal places with rounding
dll_import si32  StringToInt(const char* s,
														 ui16        length); // Set to Null to convert up to the null-char, must be supplied if string doesn't have one.

// ******************** Others ******************** //

#define string_arg(_id) char* _id, \
												ui16  _id ## Length /* Can be set to Null to scan until the EOS char */
#define NullStringArg Null, Null
#define UnpackStringArh(_id) _id, _id ## Length

dll_import bool IsLetter(char c);
dll_import bool IsWord(char* string, 
											 ui16  length); // Set to Null to scan until the EOS char
dll_import bool IsNumber(char c);
dll_import bool IsNumber(char* string, 
												 ui16  length); // Set to Null to scan until the EOS char
dll_import bool IsWhitespace(char c); // Space, horizontal & vertical tabs, carriage return, newline & feed

dll_import 			 char* // Contains am EOS char at the end. Must be freed with Free(char*)
											 AllocateString( const char* s, 
																			 ui16        length); // Set to Null to copy until and including the null-char
dll_import 			 bool  AreEqual(const char* s1, 
																      ui16  s1Length,  // Set to Null to scan until the EOS char
																const char* s2, 
																      ui16  s2Length); // Set to Null to scan until the EOS char
dll_import 			 char* // Contains a null char at the end. Must be freed with Free(char*)
											 Concatenate( // Will remove all null-chars from all strings supplied and automatically add one to the final returned string
																		ui8 count, 
																		...); // All args must be char*
dll_import 			 bool  ContainsAnySubstring(const char*  string, 
																									ui16   length, // Set to Null to scan until the EOS char
																						const char** substrings, 
																									ui8    subsCount); 
dll_import 			 void  Copy(char* source, 
														ui16  srcLength, // Set to Null to extract until the EOS char
														char* destination, 
														ui16 	destLength);
dll_import 			 char* // Contains a null char at the end. Must be freed with Free(char*)
											 ExtractSubstring(const char* string, 
																				ui16 				length); // Set to Null to extract until the EOS char. If this is larger than the actual string length, extraction will stop after the EOS char
dll_import const char* // Does not need to be freed.
											 FindSubstring(const char* sub, 
																		       ui16  subLength,     // Set to Null to scan until the EOS char
																		 const char* string,
																					 ui16  stringLength); // Set to Null to scan until the EOS char
dll_import 			 void  Free(void* string); // Only for strings allocated through this- or the memroy- APIs. Points to Free(void*) in apad_memory.cpp.
dll_import 			 ui16  // Will == Null if no EOS char is found within 300 chars from string start
											 // If not == Null, will not include the EOS
											 GetLength(const char* s);
dll_import 			 char* Push( // If only an EOS char is wanted, set string & length to Null and addEOS to true.
														const char* 	string, 
																	ui16    length, // Set to Null to scan until and including the EOS char - 1
														bool 				  addEOS, 
														memory_stack& stack);
dll_import 			 bool  StringIsEqualToAny(const char*  string, 
																								ui16   length, // Set to Null to scan until the EOS char
																					const char** strings, 
																					ui8 				 count);

#endif
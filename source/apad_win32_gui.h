#ifndef APAD_WIN32_GUI_H
#define APAD_WIN32_GUI_H

#include <windows.h>

#define DesktopAppEntryPoint(_instanceID) int CALLBACK WinMain(HINSTANCE _instanceID, HINSTANCE prevInstance, LPSTR commandLine, int commandShow)

#define BeginWin32GUIUpdateLoop() { \
																		bool _continueGUILoop = true; \
																		while (_continueGUILoop == true) { \
                                    /* The message pump will work for all windows in the calling thread */ \
                                    { extern void RunWin32GUIUpdateLoopMessagePumpInternal(); \
                                      RunWin32GUIUpdateLoopMessagePumpInternal(); } \
                                    if(GetSystemData()->runUpdateLoop == false) \
                                      break;

#define EndWin32GUIUpdateLoop(_win32UpdateLoopData) { extern void EndWin32GUIUpdateLoopInternal(); \
                                                      EndWin32GUIUpdateLoopInternal(); } \


#endif

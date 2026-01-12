#ifndef APAD_WIN32_GUI_H
#define APAD_WIN32_GUI_H

#include <windows.h>

#define DesktopAppEntryPoint(_instanceID) int CALLBACK WinMain(HINSTANCE _instanceID, HINSTANCE prevInstance, LPSTR commandLine, int commandShow)

imported_function void InitGUI(const char* windowTitle, // Can be set to Null
															 HINSTANCE instance);
															 
imported_function void BeginGUILoop();

#endif

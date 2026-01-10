#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_string.h"

// ******************** Local API start ******************** //


program_local LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	return DefWindowProcA(window, msg, wparam, lparam);
}

// ******************** Local API end ******************** //


void CreateGUIWindow(const char* windowTitle, HINSTANCE instance) {
	AssertRet(instance != Null);
	
	// @TODO - Custom program icon
	// @TODO - DPI awareness
	// @TODO - Set global GUI app flag
	// @TODO - Clock resolution for Sleep()
	// @TODO - timeEndPeriod() call when exiting app - shutdown function / intrinsic ExitProgram() call
	
	// Get the display work area first
	ui16 width = 0;
	ui16 height = 0;
	{
		RECT workArea = {};
		AssertRet(SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0) != 0);
		
		width = workArea.right - workArea.left;
		height = workArea.bottom;
	}
	AssertRet(width > 0);
	AssertRet(height > 0);
	
	string windowClassID = (string)windowTitle + " class";
	
	WNDCLASSA wndclass = {};
  // wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WindowProc;
  wndclass.hInstance = instance;
  wndclass.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW); // @TODO - Is this needed by default?
  wndclass.lpszClassName = windowClassID;
	
	AssertRet(RegisterClassA(&wndclass) != 0);
	
	HWND window = CreateWindowA(wndclass.lpszClassName, windowTitle, // @TODO - What to do if windowTitle == Null?
															WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
															0, 0, width, height, 
															NULL, NULL, instance /* @TODO - Windows documentation says this is optional */, NULL);
  AssertRet(window != NULL);
}
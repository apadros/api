#include <windows.h>
#include <gl\gl.h>

#include "apad_intrinsics.h"
#include "apad_opengl.h"
#include "apad_opengl_internal.h"
#include "apad_string.h"
#include "apad_win32_gui.h"
#include "apad_win32_internal.h"

// ******************** Internal API start ******************** //

program_local HWND windowHandle = NULL;

#include "apad_time.h"
program_local UINT 				SleepPeriod = Null;
program_local time_marker LastLoopMarker = Null;
program_local f32         Dt = Null; //Delta time since last frame, used for anything which will change over time (e.g. animations)
program_local time_marker LastLeftClickMarker = GetTimeMarker(); // GetTimeElapsedMilli() will hit an assertion if this == 0
program_local f32  				DoubleClickTime = 0.5; // Seconds

// No need to export this, only used in apad_error.cpp
dll_export program_external void Win32DisplayInfoBox(const char* string, bool error) {
	FunctionStart(;);
	
	MessageBox(NULL, string, error == true ? "Error" : "Info", MB_OK | (error == true ? MB_ICONEXCLAMATION : MB_ICONINFORMATION));
	
	FunctionEnd();
}

program_local LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
	FunctionStart(Null);
	
	if(msg == WM_CREATE) {
		// Init OpenGL
		
		windowHandle = window;
		
    PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		
		HDC dc = GetDC(window);
		int format = ChoosePixelFormat(dc, &pfd);
		AssertInternalWin32(format != 0);
		
		BOOL ret = SetPixelFormat(dc, format, &pfd);
		AssertInternalWin32(ret == TRUE);
		
		HGLRC context = wglCreateContext(dc);
		AssertInternalWin32(context != NULL);
		
		AssertInternal(wglMakeCurrent(dc, context) == TRUE);
		
		glEnable(GL_BLEND);
		AssertInternalGL();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AssertInternalGL();
		
		// Set the projection matrix based on the window client space
		auto size = Win32GetProgramWindowClientSize();
		AssertInternal(size.width > 0 && size.height > 0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		AssertInternalGL();
		glOrtho(0, size.width, 0, size.height, -1, 1);
		AssertInternalGL();
		
		ReleaseDC(window, dc);
  }
	else if(msg == WM_CLOSE) {
    // DefWindowProcA() will call DestroyWindow()
	}
  else if(msg == WM_DESTROY) {
		HDC dc = GetDC(window);
		HGLRC glrc = wglGetCurrentContext();
		
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(glrc);
		ReleaseDC(window, dc);
		
    PostQuitMessage(0); 
  }
	
	auto ret = DefWindowProcA(window, msg, wparam, lparam);
	
	FunctionEnd();
	return ret;
}

dll_export vector Win32GetProgramWindowClientSize() {
	FunctionStart(vector());
	AssertInternal(windowHandle != NULL);
	RECT r = {};
	AssertInternalWin32(GetClientRect(windowHandle, &r) != 0);
	vector ret = {};
	ret.width = r.right - r.left;
	AssertInternal(ret.width != 0);
	ret.height = r.bottom - r.top;
	AssertInternal(ret.height != 0);
	FunctionEnd();
	return ret;
}

dll_export void DisplayLastWin32Error() {
	FunctionStart(;);
	
	auto error = GetLastError();
	Win32DisplayInfoBox(ToString((ui32)error), true);
	
	FunctionEnd();
}

program_local void Win32Exit() { // Called within ExitProgram()
	// Don't care about assertions at this point
	
	if(SleepPeriod != Null); {
		auto library = LoadLibrary("Winmm.dll");
		if(library == NULL)
			return;

		MMRESULT (*timeEndPeriod)(UINT uPeriod) = (MMRESULT (*)(UINT))GetProcAddress(library, "timeEndPeriod");
		if(timeEndPeriod == NULL)
			return;
	
		auto ret = timeEndPeriod(SleepPeriod);
		if(ret != TIMERR_NOERROR)
			return;
		
		FreeLibrary(library);
	}
}

// ******************** Internal API end ******************** //

dll_export void Win32InitGUI(const char* windowTitle, HINSTANCE instance) {
	FunctionStart(;);
	
	extern bool GUIApp;
	GUIApp = true;
	
	AssertInternal(instance != Null);
	
	// DPI
  {
    auto ret = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		AssertInternalWin32(ret == TRUE);
  }

	auto library = LoadLibraryA("Winmm.dll");
	AssertInternalWin32(library != NULL);
	
	// Set the minimum os clock resolution for Sleep()
  {
    MMRESULT (*timeGetDevCaps)(LPTIMECAPS, UINT) = (MMRESULT (*)(LPTIMECAPS, UINT))GetProcAddress(library, "timeGetDevCaps");
		AssertInternalWin32(timeGetDevCaps != NULL);
		
    TIMECAPS caps = {};
		auto ret = timeGetDevCaps(&caps, sizeof(TIMECAPS));
		AssertInternalWin32(ret == MMSYSERR_NOERROR);
		AssertInternalWin32(caps.wPeriodMin == 1);
      
    MMRESULT (*timeBeginPeriod)(UINT) = (MMRESULT (*)(UINT))GetProcAddress(library, "timeBeginPeriod");
		AssertInternalWin32(timeBeginPeriod != NULL);
		
    ret = timeBeginPeriod(caps.wPeriodMin);
		AssertInternalWin32(ret == TIMERR_NOERROR);
		
		SleepPeriod = caps.wPeriodMin;
		
		atexit(Win32Exit); // @TODO - Returns 0 for no error. What to do if it doesn't?
  }
	FreeLibrary(library);
	
	// Get the display work area first
	ui16 width = 0;
	ui16 height = 0;
	{
		RECT workArea = {};
		AssertInternalWin32(SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0) != 0);
		
		width = workArea.right - workArea.left;
		height = workArea.bottom;
	}
	AssertInternalWin32(width > 0);
	AssertInternalWin32(height > 0);
	
	WNDCLASSA wndclass = {};
  // wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WindowProc;
  wndclass.hInstance = instance;
  wndclass.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW); // @TODO - Is this needed by default?
  wndclass.lpszClassName = "APAD window class";
	
	AssertInternalWin32(RegisterClassA(&wndclass) != 0);
	
	windowHandle = CreateWindowA(wndclass.lpszClassName, windowTitle,
															 WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
															 0, 0, width, height, 
															 NULL, NULL, instance /* @TODO - Windows documentation says this is optional, double check */, NULL);
  AssertInternalWin32(windowHandle != NULL);
	
	FunctionEnd();
}

#include <windowsx.h>
#include "apad_time.h"
dll_export win32_state Win32BeginGUIUpdateLoop() {
	FunctionStart(win32_state());
	
	win32_state ret = {};
	ret.lastFrameTime = Dt;
	
	MSG msg;
  ClearStruct(msg);
  while (PeekMessageA(&msg, Null, 0, 0, PM_REMOVE)) {
		bool exit = false;
		
		switch(msg.message) {
			case WM_QUIT:
				exit = true; 
				break;
				
			// WM_LBUTTONDBLCLK was attempted but it didn't work even specifying CS_DBLCLKS as a window style
			// as per the MSDN documentation.
			// Also, accoring to the docs, a WM_LBUTTONDOWN is generated first anyway, so might as well keep
			// track of double clicking manually.
				
			case WM_LBUTTONDOWN: {
				auto newMarker = GetTimeMarker();
				if(GetTimeElapsedMilli(LastLeftClickMarker, newMarker) / 1000 <= DoubleClickTime)
					ret.mouseLeftDoubleClick = true;
				LastLeftClickMarker = newMarker;
				ret.mouseLeftClickDown = true;
				ret.mouseX = GET_X_LPARAM(msg.lParam);
				ui16 height = Win32GetProgramWindowClientSize().height;
				ret.mouseY = height - GET_Y_LPARAM(msg.lParam);
			} break;
			
			case WM_LBUTTONUP: {
				ret.mouseLeftClickUp = true;
				ret.mouseX = GET_X_LPARAM(msg.lParam);
				ui16 height = Win32GetProgramWindowClientSize().height;
				ret.mouseY = height - GET_Y_LPARAM(msg.lParam);
			} break;
			
			case WM_RBUTTONDOWN: {
				ret.mouseRightClickDown = true;
				ret.mouseX = GET_X_LPARAM(msg.lParam);
				ui16 height = Win32GetProgramWindowClientSize().height;
				ret.mouseY = height - GET_Y_LPARAM(msg.lParam);
			} break;
			
			case WM_RBUTTONUP: {
				ret.mouseRightClickUp = true;
				ret.mouseX = GET_X_LPARAM(msg.lParam);
				ui16 height = Win32GetProgramWindowClientSize().height;
				ret.mouseY = height - GET_Y_LPARAM(msg.lParam);
			} break;
			
			case WM_MOUSEMOVE: {
				ret.mouseMoved = true;
				ret.mouseX = GET_X_LPARAM(msg.lParam);
				ui16 height = Win32GetProgramWindowClientSize().height;
				ret.mouseY = height - GET_Y_LPARAM(msg.lParam);
			} break;
			
			// @TODO - Test with freely-rotating mouse wheel
			case WM_MOUSEWHEEL: { // Sent outside of client area but within confines of program window
				// According to msdn documentation, WHEEL_DELTA == 120 and is used for standard mouse wheels.
				// For freely-rotating wheels, value will be different
				auto rotation = GET_WHEEL_DELTA_WPARAM(msg.wParam);
				f32  fraction = (f32)rotation / WHEEL_DELTA;
				ret.mouseWheelRotation = fraction;
			} break;
			
			case WM_KEYDOWN: // When a key that can be mapped to a char is pressed, both this and a WM_CHAR message will be generated	.
											 // Also, multiple events will be generate if a key is held down
			case WM_CHAR: { // Need this for correct mapping of text keys being pressed - context sensitive
				auto key = msg.wParam;
				bool read = false;
				switch(key) {
					case VK_BACK:   { ret.backspacePressed = true; read = true; } break;
					case VK_ESCAPE: { ret.escapePressed = true; 	 read = true; } break;				
					case VK_SPACE:  { ret.keyPressed = ' '; 	 		 read = true; } break;				
					case VK_RETURN: { ret.enterPressed = true;     read = true; } break;
					case VK_TAB:    { ret.tabPressed = true;     	 read = true; } break;
					case VK_DELETE: { ret.deletePressed = true;    read = true; } break;
					case VK_LEFT:   { ret.leftPressed = true;      read = true; } break;
					case VK_RIGHT:  { ret.rightPressed = true;     read = true; } break;
					case VK_DOWN:   { ret.downPressed = true;      read = true; } break;
					case VK_UP:     { ret.upPressed = true;     	 read = true; } break;
					default: break;
				};
				
				if(read == false && msg.message == WM_CHAR)
					ret.keyPressed = key;
			} break;
			
			
			default: break;
		};
		
		TranslateMessage(&msg);
    DispatchMessageA(&msg);
		
		if(exit == true)
			ExitProgram(false);
	}
	
	// Keyboard state
	{
		ui8 keys[256];
		BOOL win32Ret = GetKeyboardState((PBYTE)(&keys));
		AssertInternalWin32(win32Ret != 0);
		
		ret.capsLock = BitIsSet(0, keys[VK_CAPITAL]);
		ret.leftShift = BitIsSet(7, keys[VK_LSHIFT]);
		ret.rightShift = BitIsSet(7, keys[VK_RSHIFT]);
		ret.leftAlt = BitIsSet(7, keys[VK_LMENU]);
		ret.rightAlt = BitIsSet(7, keys[VK_RMENU]);
		ret.leftCtrl = BitIsSet(7, keys[VK_LCONTROL]);
		ret.rightCtrl = BitIsSet(7, keys[VK_RCONTROL]);
	}
	
	glClearColor(0, 0, 0, 0);
	AssertInternalGL();
	glClear(GL_COLOR_BUFFER_BIT);
	AssertInternalGL();
	
	FunctionEnd();
	
	return ret;
}

dll_export void Win32EndGUIUpdateLoop() {
	FunctionStart(;);
	
	if(LastLoopMarker == Null) {
		LastLoopMarker = GetTimeMarker();
		FunctionEnd();
		return;
	}

	AssertInternalWin32(windowHandle != NULL);
	if(windowHandle == NULL)
		ExitProgram(true);
	
	ui8 targetFPS = 60;
	f32 targetFrameTimeMilli = 1000.0f / targetFPS;

	auto dc = GetDC(windowHandle);
	
	f32 currentFrameTimeMilli = GetTimeElapsedMilli(LastLoopMarker, GetTimeMarker());
	if (currentFrameTimeMilli + 1 < targetFrameTimeMilli) { // Allow a 1ms threshold for Sleep() timing inaccuracy
    f32 sleepTimeMilli = targetFrameTimeMilli - (currentFrameTimeMilli + 1);
		Sleep((DWORD)sleepTimeMilli);
  }
		
  do currentFrameTimeMilli = GetTimeElapsedMilli(LastLoopMarker, GetTimeMarker());
  while (currentFrameTimeMilli < targetFrameTimeMilli);
	
	AssertInternalWin32(SwapBuffers(dc) == TRUE);
	
	Dt = GetTimeElapsedMilli(LastLoopMarker, GetTimeMarker()) / 1000;

	LastLoopMarker = GetTimeMarker();
		
	ReleaseDC(windowHandle, dc);
	
	FunctionEnd();
}

#include "apad_maths.h"
dll_export vector Win32GetMousePosWithinClient() {
	FunctionStart(vector());
	
	POINT p = {};
	AssertInternalWin32(GetCursorPos(&p) != 0); // Will return screen coordinates
	AssertInternal(windowHandle != NULL);
	AssertInternalWin32(ScreenToClient(windowHandle, &p) != 0); // Will update p relative to the top left corner of the client area
	
	auto client = Win32GetProgramWindowClientSize();
	
	// Cap the point directly since it could return negative numbers
	Clamp(p.x, 0, client.width);
	Clamp(p.y, 0, client.height);
	
	vector ret = {};
	ret.x = p.x;
	ret.y = client.height - p.y;
	
	FunctionEnd();
	return ret;
}

dll_export char* Win32OpenFileGUI(const char* directory, const char* filters) {
	FunctionStart(Null);
	AssertInternal(filters != Null);
	
	char buffer[MAX_PATH];
			
	OPENFILENAMEA data = {};
	data.lStructSize = sizeof(OPENFILENAMEA);
	data.lpstrFilter = filters;
	data.nFilterIndex = 1;
	data.lpstrFile = buffer;
	data.lpstrFile[0] = '\0';
	data.nMaxFile = MAX_PATH;
	data.lpstrInitialDir = directory;
	data.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; // Does OFN_PATHMUSTEXIST intrinsically include OFN_FILEMUSTEXIST?
	// OFN_ALLOWMULTISELECT - Will allow selection of multiple files
	
	BOOL success = GetSaveFileNameA(&data);
	char* ret = Null;
	if(success != 0) // File selected and OK clicked
		ret = AllocateString(buffer, Null);
	// 0 indicates cancel clicked or error occured, call  CommDlgExtendedError() to get error info
		
	FunctionEnd();
	return ret;
}

dll_export char* Win32SaveFileAsGUI(const char* directory, const char* filters) {
	FunctionStart(Null);
	AssertInternal(filters != Null);
	
	char buffer[MAX_PATH];
			
	OPENFILENAMEA data = {};
	data.lStructSize = sizeof(OPENFILENAMEA);
	data.lpstrFilter = filters;
	data.nFilterIndex = 1;
	data.lpstrFile = buffer;
	data.lpstrFile[0] = '\0';
	data.nMaxFile = MAX_PATH;
	data.lpstrInitialDir = directory == Null ? "." : directory;
	data.Flags = OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	
	BOOL success = GetSaveFileNameA(&data);
	char* ret = Null;
	if(success != 0) // File selected and OK clicked
		ret = AllocateString(buffer, Null);
	else // 0 indicates cancel clicked or error occured, call CommDlgExtendedError() to get error info
		return Null;
	
	// If the user types a file name and hits ok, the extension isn't automatically added to the path, need to do so manually
	if(data.nFileExtension == 0) {
		const char* extensionString = filters;
		AssertInternalWin32(data.nFilterIndex >= 1);
		ForAll(data.nFilterIndex - 1) {
			extensionString += GetLength(extensionString) + 1; // File type
			extensionString += GetLength(extensionString) + 1; // File extension
		}
		extensionString += GetLength(extensionString) + 1; // Skip to desired extension
		extensionString = GetFileExtension(extensionString) - 1; // Move to .extension
		
		ret = Concatenate(2, ret, extensionString);
	}
	
	FunctionEnd();
	return ret;
}
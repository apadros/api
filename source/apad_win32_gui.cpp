#include <windows.h>

#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_string.h"

// ******************** Internal API start ******************** //

program_local HWND windowHandle = NULL;
program_local UINT sleepPeriod = Null;

program_local LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	return DefWindowProcA(window, msg, wparam, lparam);
}

exported_function program_external void Win32Exit() { // Called within ExitProgram()
	// Don't care about assertions at this point
	
	if(sleepPeriod != Null); {
		auto library = LoadLibrary("Winmm.dll");
		if(library == NULL)
			return;

		MMRESULT (*timeEndPeriod)(UINT uPeriod) = (MMRESULT (*)(UINT))GetProcAddress(library, "timeEndPeriod");
		if(timeEndPeriod == NULL)
			return;
	
		auto ret = timeEndPeriod(sleepPeriod);
		if(ret != TIMERR_NOERROR)
			return;
		
		FreeLibrary(library);
	}
}

// ******************** Internal API end ******************** //

exported_function void Win32ErrorMessageBox(const char* string) {
	MessageBox(NULL, string, "Error", MB_OK | MB_ICONEXCLAMATION);
}

exported_function void Win32InitGUI(const char* windowTitle, HINSTANCE instance) {
	extern bool GUIApp;
	GUIApp = true;
	
	AssertRet(instance != Null);
	
	// DPI
  {
    auto ret = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		Assert(ret == TRUE);
		if(ret == FALSE) {
			auto error = GetLastError();
			DisplayErrorGUI(ToString((ui32)error));
		}
  }

	auto library = LoadLibraryA("Winmm.dll");
	AssertRet(library != NULL);
	
	// Set the minimum os clock resolution for Sleep()
  {
    MMRESULT (*timeGetDevCaps)(LPTIMECAPS, UINT) = (MMRESULT (*)(LPTIMECAPS, UINT))GetProcAddress(library, "timeGetDevCaps");
		AssertRet(timeGetDevCaps != NULL);
		
    TIMECAPS caps = {};
		auto ret = timeGetDevCaps(&caps, sizeof(TIMECAPS));
		AssertRet(ret == MMSYSERR_NOERROR);
		AssertRet(caps.wPeriodMin == 1);
      
    MMRESULT (*timeBeginPeriod)(UINT) = (MMRESULT (*)(UINT))GetProcAddress(library, "timeBeginPeriod");
		AssertRet(timeBeginPeriod != NULL);
		
    ret = timeBeginPeriod(caps.wPeriodMin);
		AssertRet(ret == TIMERR_NOERROR);
		
		sleepPeriod = caps.wPeriodMin;
  }
	FreeLibrary(library);
	
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
	
	WNDCLASSA wndclass = {};
  // wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WindowProc;
  wndclass.hInstance = instance;
  wndclass.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW); // @TODO - Is this needed by default?
  wndclass.lpszClassName = "APAD window class";
	
	AssertRet(RegisterClassA(&wndclass) != 0);
	
	windowHandle = CreateWindowA(wndclass.lpszClassName, windowTitle,
															 WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
															 0, 0, width, height, 
															 NULL, NULL, instance /* @TODO - Windows documentation says this is optional */, NULL);
  AssertRet(windowHandle != NULL);
}

#include "apad_error.h"
exported_function void Win32BeginGUIUpdateLoop() {
	MSG msg;
  ClearStruct(msg);
  while (PeekMessageA(&msg, Null, 0, 0, PM_REMOVE)) {
		bool exit = msg.message == WM_QUIT;
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
		if(exit == true)
			ExitProgram(false);
	}
}

exported_function void Win32EndGUIUpdateLoop() {
	Assert(windowHandle != NULL);
	if(windowHandle == NULL)
		ExitProgram(true);
	
	#if 0
	auto dc = GetDC(window);
	
	f32 currentFrameTimeMilli = GetTimeElapsedMilli(system->framePreMarker, GetTimeMarker());
  if (currentFrameTimeMilli + 1 < system->targetFrameTimeMilli) {
    f32 sleepTimeMilli = system->targetFrameTimeMilli - currentFrameTimeMilli;
    Sleep((DWORD)sleepTimeMilli);
  }
  
  do currentFrameTimeMilli = GetTimeElapsedMilli(system->framePreMarker, GetTimeMarker());
  while (currentFrameTimeMilli < system->targetFrameTimeMilli);
	
  SwapBuffers(dc);
  system->dt = GetTimeElapsedMilli(system->framePreMarker, GetTimeMarker()) / 1000;
  system->framePreMarker = GetTimeMarker();
  
  
  ReleaseDC(window, dc);
	#endif
}
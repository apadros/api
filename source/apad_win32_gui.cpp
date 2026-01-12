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

void InitGUI(const char* windowTitle, HINSTANCE instance) {
	SetAssertionPrinting(false);
	AssertRet(instance != Null);
	
	// @TODO - Custom program icon
	// @TODO - DPI awareness
	// @TODO - Set global GUI app flag
	// @TODO - Clock resolution for Sleep()
	// @TODO - timeEndPeriod() call when exiting app - shutdown function / intrinsic ExitProgram() call
	
	// @TODO - Check all of this
	// DPI
  {
    auto ret = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		Assert(ret == TRUE);
		if(ret == FALSE) {
			auto error = GetLastError();
			// LogError("Error code: %u\n", error); //@TODO - Allow recording of second error in global string?
		}
  }

  auto library = LoadLibrary("Winmm.dll");

	// @TODO - Check all of this
  // Set the minimum os clock resolution for Sleep()
  {
    MMRESULT (*timeGetDevCaps)(LPTIMECAPS, UINT) = (MMRESULT (*)(LPTIMECAPS, UINT))GetProcAddress("timeGetDevCaps", library);
    TIMECAPS caps = {};
	auto ret = timeGetDevCaps(&caps, sizeof(TIMECAPS));
	Assert(ret == MMSYSERR_NOERROR);
	Assert(caps.wPeriodMin == 1);
      
    MMRESULT (*timeBeginPeriod)(UINT) = (MMRESULT (*)(UINT))GetProcAddress("timeBeginPeriod", library);
    ret = timeBeginPeriod(caps.wPeriodMin);
	Assert(ret == TIMERR_NOERROR);
    
	Assert(caps.wPeriodMin <= UI8Max);
    apiData->clockMinResolution = caps.wPeriodMin;
  }
	
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

#include "apad_error.h"
bool BeginGUILoop() {
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

void EndGUILoop(HWND window) {
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
}
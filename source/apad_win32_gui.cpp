#include <windows.h>

#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_string.h"

// ******************** Internal API start ******************** //

program_local HWND windowHandle = NULL;

#include "apad_time.h"
program_local UINT 				sleepPeriod = Null;
program_local time_marker lastLoopMarker = Null;
program_local f32         dt = Null; //Delta time since last frame, used for anything which will change over time (e.g. animations)

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

const ui8 Max = 20;
struct { 
	time_marker m0;
	time_marker m1;
	time_marker m2;
	time_marker m3;
	time_marker m4;
} markers[Max];
struct { 
	f32 t0;
	f32 t1;
	f32 t2;
	f32 t3;
	f32 t4;
} times[Max];

ui8 index = 0;

exported_function void Win32EndGUIUpdateLoop() {
	markers[index].m0 = GetTimeMarker();
	
	if(lastLoopMarker == Null) {
		lastLoopMarker = GetTimeMarker();
		return;
	}

	Assert(windowHandle != NULL);
	if(windowHandle == NULL)
		ExitProgram(true);
	
	ui8 targetFPS = 60;
	f32 targetFrameTimeMilli = 1000.0f / targetFPS;

	auto dc = GetDC(windowHandle);
	
	markers[index].m1 = GetTimeMarker();
	
	f32 currentFrameTimeMilli = GetTimeElapsedMilli(lastLoopMarker, GetTimeMarker());
	if (currentFrameTimeMilli + 1 < targetFrameTimeMilli) {
    f32 sleepTimeMilli = targetFrameTimeMilli - currentFrameTimeMilli;
		Sleep((DWORD)sleepTimeMilli);
  }
	
	markers[index].m2 = GetTimeMarker();
		
  do currentFrameTimeMilli = GetTimeElapsedMilli(lastLoopMarker, GetTimeMarker());
  while (currentFrameTimeMilli < targetFrameTimeMilli);
	
	markers[index].m3 = GetTimeMarker();
	
  // SwapBuffers(dc); // @TODO - OpengGL needs to be set up first, otherwise will take ~50ms
	dt = GetTimeElapsedMilli(lastLoopMarker, GetTimeMarker()) / 1000;
	
  lastLoopMarker = GetTimeMarker();
	
	ReleaseDC(windowHandle, dc);
	
	markers[index].m4 = GetTimeMarker();
	index += 1;
	
	if(index == Max) {
		ForAll(Max) {
			auto& m = markers[it];
			if(it > 0)
				times[it].t0 = GetTimeElapsedMilli(markers[it - 1].m4, m.m0);
			times[it].t1 = GetTimeElapsedMilli(m.m0, m.m1);
			times[it].t2 = GetTimeElapsedMilli(m.m1, m.m2);
			OutputDebugStringA(ToString(times[it].t2));
			OutputDebugStringA("\n");
			times[it].t3 = GetTimeElapsedMilli(m.m2, m.m3);
			times[it].t4 = GetTimeElapsedMilli(m.m3, m.m4);
		}
		index = 0;
	}
}
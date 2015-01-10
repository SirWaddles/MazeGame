#include "graphics/graphics.h"
#include "graphics/sdlwin.h"
#include <Windows.h>
#include <SDL_syswm.h>

bool windowSizeFound = false;
int windowSizeX = 0;
int windowSizeY = 0;

GraphicsM::Vec2 GraphicsM::GetFullscreenResolution(){
	if(windowSizeFound){
		return GraphicsM::Vec2(windowSizeX, windowSizeY);
	}
	windowSizeFound = true;
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(GraphicsM::GetMainWindow(), &info);

	HMONITOR monitor = MonitorFromWindow(info.info.win.window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mInfo;
	mInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &mInfo);
	int mW = mInfo.rcMonitor.right - mInfo.rcMonitor.left;
	int mH = mInfo.rcMonitor.bottom - mInfo.rcMonitor.top;
	windowSizeX = mW;
	windowSizeY = mH;
	return GraphicsM::Vec2(mW, mH);
}

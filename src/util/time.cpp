#include "util/time.h"
#include <Windows.h>

unsigned long firstTime = 0;

unsigned long GTime::GetTimeMs(){
	DWORD time = timeGetTime();
	if(firstTime==0){
		firstTime=time;
	}
	return (unsigned int) (time-firstTime);
}

void GTime::Delay(unsigned long time){
	Sleep(time);
}
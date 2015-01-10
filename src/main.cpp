#include "graphics/graphics.h"
#include "gui/gui.h"
#include "modules/modules.h"
#include "modules/base.h"
#include "networking/networking.h"
#include "settings/savesettings.h"
#include <SDL.h>

unsigned int lastTime=0;
float frameTime=0;

void UpdateFrameTime(){
	unsigned int newTime=SDL_GetTicks();
	//printf("Now: %i\n", newTime);
	frameTime=(float(newTime)-float(lastTime))/1000.0;
	lastTime=newTime;
}

float FrameTime(){
	return frameTime;
}

bool QuitGame = false;
void ForceQuit(){
	QuitGame=true;
}

int main(int argc, char* argv[]){
	GSaveSettings::LoadSettings();
	printf("Starting...\n");
	try{
	GraphicsM::CreateMWindow();
	} catch (int e){
		printf("Error in Window Creation\n");
		if(e==-1){
			return 0;
		}
	}
	printf("Window Created\n");
	GraphicsM::LoadAllShaders();
	GuiM::CreateGUIs();

	//GNetworking::InitNetworking();

	Modules::BaseModule* baseM=new Modules::BaseModule();
	Modules::HookModule(baseM);
	Modules::SetFocus(baseM);

	SDL_Event kevent;

	while(!QuitGame){
		UpdateFrameTime();

		Modules::DoDraw();
		GuiM::RenderGUIs();
		GNetworking::PollEvents();
		Modules::DoThink();
		GraphicsM::SwapBuffers();
		while(SDL_PollEvent(&kevent)){
			if(kevent.type==SDL_QUIT){
				QuitGame=true;
				break;
			}
			Modules::DoEvent(kevent);
			Modules::DoBGEvent(kevent);
			GuiM::FireGUIEvents(kevent);
		}
	}

	if(GNetworking::IsNetworking()){
		GNetworking::DestroyNetworking();
	}

	GSaveSettings::SaveSettings();
	Modules::RemoveModule(baseM);
	delete baseM;
	GuiM::DestroyGUIs();
	GraphicsM::DestroyWindow();

	return 0;
}
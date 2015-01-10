#ifndef _MODULE_SYSTEMS_H_
#define _MODULE_SYSTEMS_H_

#include <SDL.h>

namespace Modules {
	//class IModule;
	class IModule {
	public:
		virtual void Draw()=0;
		virtual void Think()=0;
		virtual void Event(SDL_Event &kevent)=0;
		virtual void BGEvent(SDL_Event &kevent)=0;
		virtual void Select(IModule* old)=0;
		virtual void DeSelect(IModule* newm)=0;
	};

	void HookModule(IModule* module);
	void RemoveModule(IModule* module);
	void SetFocus(IModule* module);

	void DoDraw();
	void DoThink();
	void DoEvent(SDL_Event &kevent);
	void DoBGEvent(SDL_Event &kevent);
}


#endif //_MODULE_SYSTEMS_H_
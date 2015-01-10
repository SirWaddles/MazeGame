#ifndef _BASE_M_HEADER_
#define _BASE_M_HEADER_

#include "modules/modules.h"
#include <Gwen/Controls/Button.h>
#include <Gwen/Controls/WindowControl.h>

namespace Modules {

	class BaseModule : public IModule{
	public:
		virtual void Draw();
		virtual void Think();
		virtual void Event(SDL_Event &kevent);
		virtual void BGEvent(SDL_Event &kevent);
		virtual void Select(IModule* oldm);
		virtual void DeSelect(IModule* newm);

		BaseModule();

		void ShowOptions(bool show);
	protected:
		Gwen::Controls::Button* bStart;
		Gwen::Controls::WindowControl* pWindow;

		Gwen::Controls::WindowControl* pOptions;
	private:

	};

}



#endif // _BASE_M_HEADER_
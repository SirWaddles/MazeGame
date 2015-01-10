#ifndef _GAMEM_HEADER_
#define _GAMEM_HEADER_

#include "modules/base.h"

namespace Modules{

	class MainGame : public IModule {
	public:
		void Draw();
		void Think();
		void Event(SDL_Event &kevent);
		void BGEvent(SDL_Event &kevent);
		void Select(IModule* oldm);
		void DeSelect(IModule* newm);
		void SetActiveMenu(BaseModule* nMenu);
		void CreateResources();
		void SendData();
		void UpdateModelPositions();
		MainGame();
		MainGame(bool multi);
		~MainGame();

		void CreateMaze(int w, int h);
	protected:
		BaseModule* activeMenu;
		
		int PlayX, PlayY;
		bool Multiplayer;
	private:

	};

}

#endif //_GAMEM_HEADER_
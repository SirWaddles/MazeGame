#ifndef _CONNECT_STATE_H_
#define _CONNECT_STATE_H_

#include "modules/game.h"
#include <string>

enum EConnectState{
	ECONNECT_NONE=0,
	ECONNECT_RESOLVE,
	ECONNECT_CONNECTING,
	ECONNECT_SUCCESS,
	ECONNECT_FAIL
};

void SetNewGame(Modules::MainGame* nGame);

class ConnectingState : public Modules::IModule {
public:
	void Draw();
	void Think();
	void Event(SDL_Event &kevent);
	void BGEvent(SDL_Event &kevent);

	void Select(Modules::IModule* oldm);
	void DeSelect(Modules::IModule* newm);

	ConnectingState(std::string ipAddy, std::string username);

	void SetMenu(Modules::BaseModule* theMenu);

protected:
	Modules::BaseModule* activeMenu;
	unsigned int createTime;
	EConnectState connectState;
};





#endif // _CONNECT_STATE_H_
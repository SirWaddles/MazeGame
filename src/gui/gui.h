#ifndef _GUI_HEADER_
#define _GUI_HEADER_

#include <SDL.h>
#include <string>
#include <Gwen/Controls/Canvas.h>

namespace GuiM{

	void CreateGUIs();
	void DestroyGUIs();
	void RenderGUIs();
	void FireGUIEvents(SDL_Event &kevent);
	Gwen::Controls::Canvas* GetMainCanvas();
	void SetCanvasSize(int x, int y);

	void MakeMessageBox(std::string title, std::string message);

}



#endif //_GUI_HEADER_
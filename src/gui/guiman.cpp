#include "gui/gui.h"
#include "graphics/graphics.h"

#include <Gwen/Renderers/OpenGL.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/Controls/Canvas.h>
#include <Gwen/Input/SDL13.h>

#include <Gwen/Controls/WindowControl.h>

namespace GuiM{
	Gwen::Renderer::OpenGL* pRenderer = NULL;
	Gwen::Skin::TexturedBase *skin = NULL;
	//Gwen::Skin::Simple* skin = NULL;
	Gwen::Controls::Canvas *pCanvas = NULL;
	Gwen::Input::SDL13* pInput = NULL;

	void CreateGUIs(){
		pRenderer = new Gwen::Renderer::OpenGL();
		skin = new Gwen::Skin::TexturedBase(pRenderer);
		//skin = new Gwen::Skin::Simple(pRenderer);
		skin->SetRender(pRenderer);
		skin->Init("DefaultSkin.png");
		skin->SetDefaultFont(L"OpenSans.ttf", 11);

		pCanvas = new Gwen::Controls::Canvas(skin);
		pCanvas->SetSize(GraphicsM::ScrW(), GraphicsM::ScrH());
		//pCanvas->SetDrawBackground(true);
		//pCanvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));

		pInput = new Gwen::Input::SDL13();
		pInput->Initialize(pCanvas);

	}

	void DestroyGUIs(){
		//printf("1.\n");
		delete pCanvas;
		//printf("2\n");
		delete pInput;
		//printf("3 %p\n", skin);
		// TODO: FIX ME!
		delete skin;
		//printf("4\n");
		delete pRenderer;
		//printf("5\n");
	}

	void RenderGUIs(){
		GraphicsM::DisableDepth();
		pCanvas->RenderCanvas();
	}

	void FireGUIEvents(SDL_Event &kevent){
		pInput->ProcessEvent(&kevent);
	}

	Gwen::Controls::Canvas* GetMainCanvas(){
		return pCanvas;
	}


	void SetCanvasSize(int x, int y){
		pCanvas->SetSize(x, y);
	}

	class OkayMessage : public Gwen::Event::Handler{
	public:
		void DoOkay(Gwen::Controls::Base* pBase){
			//Gwen::Controls::WindowControl* pMBox = dynamic_cast<Gwen::Controls::WindowControl*>(pBase);
			pBase->GetParent()->DelayedDelete();
		}
	};

	void MakeMessageBox(std::string title, std::string message){
		Gwen::Controls::WindowControl* pMessageBox = new Gwen::Controls::WindowControl(GuiM::GetMainCanvas());
		int xPos = int(floor(float(GraphicsM::ScrW())/2.0f))-150;
		int yPos = int(floor(float(GraphicsM::ScrH())/2.0f))-100;
		pMessageBox->SetDeleteOnClose(true);

		pMessageBox->SetBounds(xPos, yPos, 300, 200);
		pMessageBox->SetTitle(title);

		Gwen::Controls::Label* pMsgText = new Gwen::Controls::Label(pMessageBox);
		pMsgText->SetText(message);
		pMsgText->SetBounds(10, 10, 290, 50);

		Gwen::Controls::Button* pOkay = new Gwen::Controls::Button(pMessageBox);
		pOkay->SetBounds(100, 120, 100, 40);
		pOkay->SetText("Okay");
		pOkay->onPress.Add(pOkay, &OkayMessage::DoOkay);
	}

}
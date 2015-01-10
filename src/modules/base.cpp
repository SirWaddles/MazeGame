#include "modules/base.h"
#include "modules/game.h"
#include "modules/connect.h"
#include "graphics/graphics.h"
#include "gui/gui.h"
#include "delta.h"
#include "util/settings.h"
#include "settings/savesettings.h"

#include "networking/networking.h"
#include "util/srl_types.h"

#include <GL/glew.h>

#include <cmath>
#include <cstring>

#include <Gwen/Controls/HorizontalSlider.h>
#include <Gwen/Controls/ComboBox.h>
#include <Gwen/Controls/CheckBox.h>

using namespace GraphicsM;

class MainMenuEvent : public Gwen::Event::Handler{
public:
	void QuitGame();
	void StartGame();
	void OpenOptions();
	void SensitivityChanged(Gwen::Controls::Base* control);
	void ConnectToGame(Gwen::Controls::Base* control);
	void ApplySettings();
	void TryConnect(Gwen::Controls::Base* control);
	static Modules::MainGame* activeGame;
	static Modules::BaseModule* activeMenu;
};

Modules::MainGame* MainMenuEvent::activeGame = NULL;
Modules::BaseModule* MainMenuEvent::activeMenu = NULL;

void MainMenuEvent::QuitGame(){
	ForceQuit();
}

void MainMenuEvent::StartGame(){
	if(!activeGame){
		Modules::MainGame* newGame = new Modules::MainGame();
		activeGame = newGame;
	}
	Modules::HookModule(activeGame);
	Modules::SetFocus(activeGame);
}

void MainMenuEvent::OpenOptions(){
	activeMenu->ShowOptions(true);
}

void SetNewGame(Modules::MainGame* nGame){
	MainMenuEvent::activeGame = nGame;
}

Gwen::Controls::ComboBox* pRes = NULL;
Gwen::Controls::CheckBoxWithLabel* pFullscreen = NULL;

void MainMenuEvent::ApplySettings(){
	std::string sDex = pRes->GetSelectedItem()->GetName();
	int nDex = sDex[0];
	int sW = GSettings::GetResX(nDex);
	int sH = GSettings::GetResY(nDex);
	printf("Resolution %i %i\n", sW, sH);
	bool fullscreen = pFullscreen->Checkbox()->IsChecked();
	if(fullscreen){
		printf("Fullscreen\n");
	} else {
		printf("Windowed\n");
	}
	//std::string fResVal;
	char toApply[20];
	sprintf(toApply, "%ix%i", sW, sH);
	GSaveSettings::SetSetting("resolution", toApply);
	if(fullscreen){
		GSaveSettings::SetSetting("fullscreen", "true");
	} else {
		GSaveSettings::SetSetting("fullscreen", "false");
	}

	GraphicsM::SetScreenSize(sW, sH, fullscreen);
}

void MainMenuEvent::SensitivityChanged(Gwen::Controls::Base* control){
	Gwen::Controls::HorizontalSlider* nSlider = dynamic_cast<Gwen::Controls::HorizontalSlider*>(control);
	if(!nSlider) return;
	//printf("Stuff: %f\n", nSlider->GetFloatValue());
	GSettings::SetSensitivity(nSlider->GetFloatValue());
	char resStr[20];
	sprintf(resStr, "%f", nSlider->GetFloatValue());
	GSaveSettings::SetSetting("sensitivity", resStr);
}

Gwen::Controls::WindowControl* pConnect = NULL;
Gwen::Controls::TextBox* pIpAddy = NULL;
Gwen::Controls::TextBox* tName = NULL;

void MainMenuEvent::ConnectToGame(Gwen::Controls::Base* control){
	pConnect->Show();
}

void MainMenuEvent::TryConnect(Gwen::Controls::Base* control){
	//printf("Sending\n");
	if(MainMenuEvent::activeGame){
		delete MainMenuEvent::activeGame;
		Modules::RemoveModule(MainMenuEvent::activeGame);
		MainMenuEvent::activeGame = NULL;
	}
	GSaveSettings::SetSetting("last-server", pIpAddy->GetText().Get());
	GSaveSettings::SetSetting("username", tName->GetText().Get());
	ConnectingState* nState = new ConnectingState(pIpAddy->GetText().Get(), tName->GetText().Get());
	Modules::HookModule(nState);
	Modules::SetFocus(nState);
}

namespace Modules{

	void BaseModule::Draw(){
		
		GraphicsM::ClearWindow(Colour(0,0,0,255));

	}

	void BaseModule::Think(){

	}

	void BaseModule::Event(SDL_Event &kevent){

	}

	void BaseModule::BGEvent(SDL_Event &kevent){

	}

	void BaseModule::Select(IModule* oldm){
		pWindow->Show();
	}

	void BaseModule::DeSelect(IModule* newm){
		MainGame* newGame = dynamic_cast<MainGame*>(newm);
		if(newGame!=NULL){
			bStart->SetText("Continue");
			newGame->SetActiveMenu(this);
		}
		ConnectingState* newConnect = dynamic_cast<ConnectingState*>(newm);
		if(newConnect!=NULL){
			bStart->SetText("Continue");
			newConnect->SetMenu(this);
		}
		pWindow->Hide();
		pOptions->Hide();
		pConnect->Hide();
	}

	

	BaseModule::BaseModule(){

		MainMenuEvent::activeMenu = this;
		pWindow = new Gwen::Controls::WindowControl(GuiM::GetMainCanvas());
		pWindow->SetBounds(10, 10, 200, 460);
		pWindow->SetTitle("Main Menu");
		pWindow->SetClosable(false);
		//pWindow->DisableResizing();

		bStart = new Gwen::Controls::Button(pWindow);
		bStart->SetBounds(10,10,160,40);
		bStart->SetText("Start");
		bStart->onPress.Add(bStart, &MainMenuEvent::StartGame);

		Gwen::Controls::Button* bOptions = new Gwen::Controls::Button(pWindow);
		bOptions->SetBounds(10, 60, 160, 40);
		bOptions->SetText("Options");
		bOptions->onPress.Add(bOptions, &MainMenuEvent::OpenOptions);

		Gwen::Controls::Button* bQuit = new Gwen::Controls::Button(pWindow);
		bQuit->SetBounds(10, 160, 160, 40);
		bQuit->SetText("Quit Game");
		bQuit->onPress.Add(bQuit, &MainMenuEvent::QuitGame);

		pOptions = new Gwen::Controls::WindowControl(GuiM::GetMainCanvas());
		pOptions->SetBounds(220, 50, 400, 400);
		pOptions->SetTitle("Options");
		pOptions->SetClosable(true);
		
		pOptions->SetDeleteOnClose(false);
		//pOptions->DisableResizing();
		pOptions->Hide();

		Gwen::Controls::HorizontalSlider* pSensitivity = new Gwen::Controls::HorizontalSlider(pOptions);
		pSensitivity->SetRange(1.0f, 20.0f);
		pSensitivity->SetBounds(10, 30, 200, 30);
		std::string sensStr = GSaveSettings::GetSetting("sensitivity");
		float setSens = atof(sensStr.c_str());
		GSettings::SetSensitivity(setSens);
		pSensitivity->SetFloatValue(setSens);
		pSensitivity->onValueChanged.Add(pSensitivity, &MainMenuEvent::SensitivityChanged);

		Gwen::Controls::Label* pSensLbl = new Gwen::Controls::Label(pOptions);
		pSensLbl->SetPos(10, 20);
		pSensLbl->SetText("Sensitivity:");
		pSensLbl->SizeToContents();
		
		Gwen::Controls::Label* pSensLbl1 = new Gwen::Controls::Label(pOptions);
		pSensLbl1->SetPos(10, 60);
		pSensLbl1->SetText("1.0");
		pSensLbl1->SizeToContents();

		Gwen::Controls::Label* pSensLbl2 = new Gwen::Controls::Label(pOptions);
		pSensLbl2->SetPos(190, 60);
		pSensLbl2->SetText("20.0");
		pSensLbl2->SizeToContents();

		Gwen::Controls::Label* pResLbl = new Gwen::Controls::Label(pOptions);
		pResLbl->SetPos(10, 100);
		pResLbl->SetText("Resolution:");
		pResLbl->SizeToContents();

		pRes = new Gwen::Controls::ComboBox(pOptions);
		pRes->SetBounds(10, 120, 200, 25);
		

		int count = GSettings::GetResCount();
		int index = GSettings::GetResIndex();
		//int index = 0;

		for(int i=0; i<count; i++){
			std::string pName = "a";
			pName[0]=index+i;
			pRes->AddItem(GSettings::GetResName(index+i), pName);
		}

		pFullscreen = new Gwen::Controls::CheckBoxWithLabel(pOptions);
		pFullscreen->SetPos(10, 150);
		pFullscreen->Label()->SetText("Fullscreen");
		pFullscreen->Label()->SizeToContents();
		pFullscreen->SetWidth(100);

		Gwen::Controls::Button* pApply = new Gwen::Controls::Button(pOptions);
		pApply->SetBounds(110, 180, 100, 25);
		pApply->SetText("Apply");
		pApply->onPress.Add(pApply, &MainMenuEvent::ApplySettings);

		Gwen::Controls::Button* bConnect = new Gwen::Controls::Button(pWindow);
		bConnect->SetBounds(10, 110, 160, 40);
		bConnect->SetText("Connect to Game");
		bConnect->onPress.Add(bConnect, &MainMenuEvent::ConnectToGame);

		pConnect = new Gwen::Controls::WindowControl(GuiM::GetMainCanvas());
		pConnect->SetBounds(100, 100, 450, 200);
		pConnect->SetTitle("Connect to Network Game");
		pConnect->SetDeleteOnClose(false);

		Gwen::Controls::Label* lblIp = new Gwen::Controls::Label(pConnect);
		lblIp->SetPos(10, 10);
		lblIp->SetText("IP Address:");
		lblIp->SizeToContents();

		pIpAddy = new Gwen::Controls::TextBox(pConnect);
		pIpAddy->SetBounds(10, 25, 400, 30);
		pIpAddy->SetText(GSaveSettings::GetSetting("last-server"));

		Gwen::Controls::Button* bLoad = new Gwen::Controls::Button(pConnect);
		bLoad->SetBounds(175, 120, 100, 40);
		bLoad->SetText("Connect");
		bLoad->onPress.Add(bLoad, &MainMenuEvent::TryConnect);

		Gwen::Controls::Label* lblName = new Gwen::Controls::Label(pConnect);
		lblName->SetPos(10, 60);
		lblName->SetText("Name:");
		lblName->SizeToContents();

		tName = new Gwen::Controls::TextBox(pConnect);
		std::string usernameStr = GSaveSettings::GetSetting("username");
		tName->SetText(usernameStr);
		tName->SetBounds(10, 75, 400, 30);
		pConnect->Hide();

		//GNetworking::AddDecoder(NDecoder(&ReceiveConnection, 1));
		

		ShaderProgram* shader=GraphicsM::GetShader("default");
		unsigned int scrID=shader->GetUniformID("scrSize");
		GraphicsM::UseShader(shader);
		glUniform2f(scrID, GraphicsM::ScrW(), GraphicsM::ScrW());
		GraphicsM::NullShader();

	}

	void BaseModule::ShowOptions(bool show){
		if(show){
			pOptions->Show();
		} else {
			pOptions->Hide();
		}
	}

}
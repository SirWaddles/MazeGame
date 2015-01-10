#include "modules/connect.h"
#include "graphics/graphics.h"
#include "networking/networking.h"
#include "util/srl_types.h"
#include "gui/gui.h"

using namespace Modules;

GraphicsM::TFont* nFont2 = NULL;
bool decoderAdded = false;

BaseModule* gNMenu = NULL;

void ReceiveConnection(const std::string& eMsg){
	unsigned int mSeed = SrlT::SetInt(eMsg.substr(3, 4));
	unsigned short mWidth = SrlT::SetShort(eMsg.substr(7, 2));
	unsigned short mHeight = SrlT::SetShort(eMsg.substr(9, 2));
	int myIndex = atoi(eMsg.substr(2, 1).c_str());
	ClientSettings::SetMyIndex(myIndex);
	ServerSettings::SetMazeSeed(mSeed);
	ServerSettings::SetMazeWidth(mWidth);
	ServerSettings::SetMazeHeight(mHeight);
	std::string sNames = eMsg.substr(11);
	std::size_t found = sNames.find_first_of("*");
	std::size_t foundLast = 0;
	printf("Received Connection: %s\n", eMsg.c_str());
	while(found!=std::string::npos){
		std::string nPly = sNames.substr(foundLast, found-foundLast);
		//printf("PlyString: %s\n", nPly.c_str());
		foundLast=found;
		found = sNames.find_first_of("*", found+1);
		int index = atoi(nPly.substr(0, 1).c_str());
		unsigned short score = SrlT::SetShort(nPly.substr(1, 2));
		GPlayers::AddPlayer(index, nPly.substr(3));
		PlayerT* ply = GPlayers::GetPlayer(index);
		ply->score = score;
	}
	Modules::MainGame* newGame = new Modules::MainGame(true);
	SetNewGame(newGame);
	Modules::HookModule(newGame);
	Modules::SetFocus(newGame);
	newGame->SetActiveMenu(gNMenu);
}

void ReceiveChallenge(const std::string& eMsg){
	unsigned short plyID = SrlT::SetShort(eMsg.substr(2, 2));
	std::string toSend = SrlT::GetShort(12);
	toSend += SrlT::GetShort(plyID);
	GNetworking::SendToServer(toSend);
}

void ConnectingState::Draw(){
	GraphicsM::ClearWindow(GraphicsM::Colour(0,0,0,255));
	unsigned int toNow = SDL_GetTicks() % 3000;
	std::string toWrite = "CONNECTING.";
	if(toNow>1000 && toNow<2000){
		toWrite="CONNECTING..";
	}
	if(toNow>=2000){
		toWrite = "CONNECTING...";
	}
	GraphicsM::FontSize fSize = nFont2->MeasureText(toWrite.c_str());
	float xPos = float(GraphicsM::ScrW())/2.0 - float(fSize.w)/2.0;
	float yPos = float(GraphicsM::ScrH())/2.0 - float(fSize.h)/2.0;
	nFont2->RenderText(xPos, yPos, toWrite.c_str(), GraphicsM::Colour(255, 255, 255, 255));
}

void ConnectingState::Think(){
	unsigned int toNow = SDL_GetTicks();
	if(toNow > createTime + 5000){
		GuiM::MakeMessageBox("Error", "Could not connect to server.");
		Modules::SetFocus(activeMenu);
	}
}

void ConnectingState::Event(SDL_Event &kevent){

}

void ConnectingState::BGEvent(SDL_Event &kevent){

}

void ConnectingState::Select(IModule* oldm){

}

void ConnectingState::DeSelect(IModule* newm){
	Modules::RemoveModule(this);
	delete this;
}

ConnectingState::ConnectingState(std::string ipAddy, std::string username){
	if(!decoderAdded){
		GNetworking::AddDecoder(NDecoder(&ReceiveConnection, 1));
		GNetworking::AddDecoder(NDecoder(&ReceiveChallenge, 4));
		decoderAdded = true;
	}

	GNetworking::InitNetworking();
	GNetworking::ConnectToServer(ipAddy, 13);
	std::string eMsg = SrlT::GetShort(0);
	eMsg += username;
	
	GNetworking::SendToServer(eMsg);
	GNetworking::StartListening();
	connectState = ECONNECT_CONNECTING;
	createTime = SDL_GetTicks();
	
	if(!nFont2){
		nFont2 = GraphicsM::GetFont("OpenSans.ttf", 11);
	}
}

void ConnectingState::SetMenu(BaseModule* theMenu){
	activeMenu = theMenu;
	gNMenu = theMenu;
}
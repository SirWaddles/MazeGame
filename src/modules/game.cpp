#include "modules/game.h"
#include "maze/walls.h"
#include "delta.h"
#include "util/settings.h"
#include "graphics/scene.h"
#include "graphics/particles.h"

#include "networking/networking.h"
#include "util/srl_types.h"

#include "gui/gui.h"
#include <Gwen/Controls/TextBox.h>

#include <SDL_mouse.h>

#include <cmath>
#include <list>
#include <deque>

#define PI 3.14159265

using namespace Modules;
using namespace GraphicsM;

Maze::MazeGeometry MazeModel;
Scene* MazeScene = NULL;

float playDir=90.0f;
float playYDir=0.0f;

float playX = 1.5f;
float playY = 1.5f;

ModelInstance* FinishTrophy = NULL;
TFont* nFont = NULL;
Model* playerModel = NULL;
Material* playerMaterial = NULL;

ModelInstance* MazeModelInst = NULL;
ModelInstance* FloorMdlInst = NULL;
ModelInstance* CeilingMdlInst = NULL;

bool ExitFound = false;

bool drawLeaderboard = false;
std::list<PlayerT*> sortedPlyList;

struct ChatMsg{
	unsigned int msgTime;
	std::string cMsg;
};

std::deque<ChatMsg> chatHistory;

ModelInstance* MPlayers[MAX_PLAYERS] = {NULL};
int lastData = 0;
Gwen::Controls::TextBox* chatBox = NULL;

GParticles::TParticleEmitter* partEmitter = NULL;

MainGame::~MainGame(){
	delete MazeModel.FloorModel;
	delete MazeModel.MazeModel;
	delete MazeModel.CeilingModel;
	MazeModel = Maze::MazeGeometry();
	playDir=0.0f;
	playYDir=0.0f;
	playX=1.5f;
	playY=1.5f;

	//delete mainLight;
	//mainLight = NULL;

	delete FinishTrophy->model;
	delete FinishTrophy->material->matTexture;
	delete FinishTrophy->material;
	//delete FinishTrophy;
	FinishTrophy = NULL;

	nFont = NULL;

	delete playerModel;
	playerModel = NULL;

	delete playerMaterial->matTexture;
	delete playerMaterial;
	playerMaterial = NULL;

	delete MazeModelInst->material->matTexture;
	delete FloorMdlInst->material->matTexture;
	delete MazeModelInst->material;
	delete FloorMdlInst->material;
	delete CeilingMdlInst->material->matTexture;
	delete CeilingMdlInst->material;
	//delete MazeModelInst;
	//delete FloorMdlInst;
	MazeModelInst = NULL;
	FloorMdlInst = NULL;
	CeilingMdlInst = NULL;

	if(Multiplayer){
		chatBox->DelayedDelete();
		chatBox = NULL;
	}

	delete MazeScene;
	MazeScene = NULL;

	sortedPlyList.clear();
	chatHistory.clear();
	for(int i=0; i<MAX_PLAYERS; i++){
		MPlayers[i]=NULL;
	}
	lastData = 0;

	if(Multiplayer){
		GNetworking::RemoveDecoder(3);
		GNetworking::RemoveDecoder(5);
		GNetworking::RemoveDecoder(7);
		GNetworking::RemoveDecoder(8);
		GNetworking::RemoveDecoder(10);
		GNetworking::RemoveDecoder(11);
	}
}

void MainGame::Draw(){
	GraphicsM::DisableAlpha();
	GraphicsM::ClearWindow(GraphicsM::Colour(100,100,100,255));
	//GraphicsM::ClearDepth();

	LoadMIdentity();
	RotateM(playYDir-90.0f, 1.0f, 0.0f, 0.0f);
	RotateM(playDir, 0.0f, 0.0f, 1.0f);
	TranslateM(-playX, -playY, -0.5f);

	//Vec3 SPos = GraphicsM::GetSSpace(Vec3(1.5f, 1.5f, 0.5f));
	//printf("SPos: %f\n", SPos.z);
	

	MazeScene->DrawScene();

	partEmitter->DrawParticles();

	if(ExitFound){
		nFont->RenderText(10.0f, 10.0f, "Exit Found!", Colour(255, 255, 255, 255));
	}

	int chatMsgs = chatHistory.size();
	if(chatMsgs>0){
		int sSize = chatMsgs*20;
		int count=0;
		std::deque<ChatMsg>::iterator it;
		for(it=chatHistory.begin(); it<chatHistory.end(); it++){
			int y=GraphicsM::ScrH()+(count*20)-120-sSize;
			count++;
			nFont->RenderText(10.0f, float(y), it->cMsg.c_str(), Colour(255, 255, 255, 255));
		}

	}
	
	for(int i=0; i<MAX_PLAYERS; i++){
		PlayerT* ply = GPlayers::GetPlayer(i);
		if(i == ClientSettings::GetMyIndex()) continue;
		if(!ply) continue;
		Vec3 SPos = GraphicsM::GetSSpace(Vec3(ply->x, ply->y, 0.5f));
		float w = GraphicsM::GetLastW();
		if(w>0.1f){
			int alpha = (int)floor((5.0f-SPos.z)*50.0f);
			if(alpha>=255) alpha = 255;
			if(alpha>0)
			nFont->RenderText(SPos.x, -SPos.y, ply->name.c_str(), Colour(255, 255, 255, alpha));
		}
		//nFont->RenderText(SPos.x, SPos.y, ply->name.c_str(), Colour(255, 255, 255, 255));
	}

	if(drawLeaderboard){
		//printf("WAAAT\n");
		std::list<PlayerT*>::iterator it;
		float yDraw = 100.0f;
		float xDrawName = (float(GraphicsM::ScrW())/2.0f) - 100.0f;
		float xDrawScore = xDrawName + 300.0f;

		for(it=sortedPlyList.begin(); it!=sortedPlyList.end(); it++){
			printf("Printing: %s\n", (*it)->name.c_str());
			PlayerT* ply = (*it);
			nFont->RenderText(xDrawName, yDraw, ply->name.c_str(), GraphicsM::Colour(255, 255, 255, 255));
			char nScore[5];
			itoa(ply->score, nScore, 10);
			nFont->RenderText(xDrawScore, yDraw, nScore, GraphicsM::Colour(255, 255, 255, 255));
			yDraw += 20.0f;
		}
	}

}

void AddChatMessage(std::string message){
	ChatMsg toAdd;
	unsigned int now = SDL_GetTicks();
	toAdd.msgTime = now;
	toAdd.cMsg = message;
	chatHistory.push_back(toAdd);
}

void MainGame::UpdateModelPositions(){
	for(int i=0; i<MAX_PLAYERS; i++){
		PlayerT* ply = GPlayers::GetPlayer(i);
		if(i == ClientSettings::GetMyIndex()){
			continue;
		}
		if(!ply){
			if(MPlayers[i]){
				MazeScene->RemoveModel(MPlayers[i]);
				MPlayers[i]=NULL;
			}
			continue;
		}
		if(!MPlayers[i]){
			ModelInstance* newPly = new ModelInstance();
			newPly->material = playerMaterial;
			newPly->Scale = Vec3(1.0f, 1.0f, 1.0f);
			newPly->Position = Vec3(ply->x, ply->y, 0.0f);
			newPly->model = playerModel;
			MazeScene->AddModel(newPly);
			MPlayers[i] = newPly;
			continue;
		}
		MPlayers[i]->Position = Vec3(ply->x, ply->y, 0.0f);
		Quaternion playDirQ;
		float term = ((ply->dir+90.0f)*PI)/360.0f;
		float sTerm = sin(term);
		float cTerm = cos(PI/2.0f);
		playDirQ.x = sTerm*cTerm;
		playDirQ.y = sTerm*cTerm;
		playDirQ.z = sTerm;
		playDirQ.w = cos(term);
		MPlayers[i]->Angle = playDirQ;
	}
}

Vec3 GetLocalPlayerPosition(){
	return Vec3(playX, playY, 0.5f);
}

class ChatBoxEvent : public Gwen::Event::Handler {
public:
	void SubmitMessage(Gwen::Controls::Base* control);
};

void MainGame::Think(){

	unsigned int time = SDL_GetTicks();
	Quaternion lom;
	float term = (float(time % 2000)/2000.0f)*PI;
	float sTerm = sin(term);
	lom.x = sTerm*cos(PI/2.0f);
	lom.y = sTerm*cos(PI/2.0f);
	lom.z = sTerm;
	
	lom.w = cos(term);

	FinishTrophy->Angle = lom;

	UpdateModelPositions();

	if(!chatHistory.empty()){
		ChatMsg curMsg = chatHistory.front();
		if(curMsg.msgTime+5000<time){
			chatHistory.pop_front();
		}
	}


	Uint8* state = SDL_GetKeyboardState(NULL);
	float nPlayX = 0.0f;
	float nPlayY = 0.0f;

	float playRad = playDir*(PI/180.0f);
	
	int count = 0;
	if(state[SDL_SCANCODE_W]){
		nPlayX += sin(playRad);
		nPlayY += cos(playRad);
		count++;
	}
	if(state[SDL_SCANCODE_A]){
		nPlayX += sin(playRad-(PI/2.0f));
		nPlayY += cos(playRad-(PI/2.0f));
		count++;
	}
	if(state[SDL_SCANCODE_D]){
		nPlayX += sin(playRad+(PI/2.0f));
		nPlayY += cos(playRad+(PI/2.0f));
		count++;
	}
	if(state[SDL_SCANCODE_S]){
		nPlayX += sin(playRad+PI);
		nPlayY += cos(playRad+PI);
		count++;
	}

	if(Multiplayer && !chatBox->Hidden()){
		count = 0;
	}

	if(count>0){

		nPlayX = nPlayX / float(count);
		nPlayY = nPlayY / float(count);

		nPlayX = nPlayX*FrameTime()*8.0f;
		nPlayY = nPlayY*FrameTime()*8.0f;

		int cX = floor(playX+nPlayX);
		int cY = floor(playY+nPlayY);
		

		Block* xBlock = Maze::GetBlock(floor(playX+nPlayX), playY);
		Block* yBlock = Maze::GetBlock(playX, floor(playY+nPlayY));

		if(xBlock && xBlock->GetBlockType() != BLOCK_WALL){
			playX+=nPlayX*0.5f;
		}
		if(yBlock && yBlock->GetBlockType() != BLOCK_WALL){
			playY+=nPlayY*0.5f;
		}

		if(Multiplayer){
			if(time > (lastData+30)){
				SendData();
				lastData = time;
			}
		}
		
		Block* cBlock = Maze::GetBlock(cX, cY);
		if(cBlock && cBlock->GetBlockType()==BLOCK_FINISH){
			ExitFound=true;
		} else {
			ExitFound=false;
		}

		/*if(cBlock && cBlock->GetBlockType()!=BLOCK_WALL){
			playX += nPlayX;
			playY += nPlayY;
		}*/
	}

	for(int i=0; i<5; i++){
		GParticles::TParticle* nPart = new GParticles::TParticle();
		nPart->SetColour(Colour(200, 50, 50, 50));
		nPart->SetPosition(Vec3(1.5f, 1.5f, 0.5f));

		float randFX = float(rand())/float(RAND_MAX);
		float randFY = float(rand())/float(RAND_MAX);
		float randFZ = float(rand())/float(RAND_MAX);

		nPart->SetDirection(Vec3((randFX*2.0f)-1.0f, (randFY*2.0f)-1.0f, (randFZ*2.0f)-1.0f));
		nPart->SetLife(500);
		nPart->SetSize(0.2f, 0.0f);
	
		partEmitter->AddParticle(nPart);
	}

	partEmitter->Think();

	//mainLight->lPos = Vec3(floor(playX)+0.5f, floor(playY)+0.5f, 0.7f);
	//mainLight->lPos = Vec3(playX, playY, 0.5f);
	//mainLight->lPos = Vec3(1.5f, 1.5f, 0.8f);
	//
	//float lightX = sin(playRad);
	//float lightY = cos(playRad);
	//
	//mainLight2->lPos = Vec3(playX+(lightX*1.0f), playY+(lightY*1.0f), 0.7f);
	//mainLight3->lPos = Vec3(playX+(lightX*-1.0f), playY+(lightY*-1.0f), 0.7f);
}

void MainGame::SendData(){
	int index = ClientSettings::GetMyIndex();
	char buf[3];
	itoa(index, buf, 10);
	
	std::string Msg = SrlT::GetShort(2);
	
	Msg += buf;
	Msg += SrlT::GetFloat(playX);
	Msg += SrlT::GetFloat(playY);
	Msg += SrlT::GetFloat(playDir);

	GNetworking::SendToServer(Msg);

}

void ReceiveCChat(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("07")!=0) return;
	//printf("eMsg: %s\n", eMsg.c_str());
	int playerID = atoi(eMsg.substr(2,1).c_str());
	PlayerT* ply = GPlayers::GetPlayer(playerID);
	std::string message;
	if(!ply){
		message = "Unknown: ";	
	} else {
		message = ply->name;
		message += ": ";
	}
	message += eMsg.substr(3);
	AddChatMessage(message);
	//printf("Test\n");
}

void UpdatePositionData(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("03")!=0) return;
	int plyCount = floor(float(eMsg.size()-2)/13.0f);

	for(int i=0; i<plyCount; i++){
		int plyIndex = atoi(eMsg.substr(2+(i*13), 1).c_str());
		float x = SrlT::SetFloat(eMsg.substr(3+(i*13), 4));
		float y = SrlT::SetFloat(eMsg.substr(7+(i*13), 4));
		float dir = SrlT::SetFloat(eMsg.substr(11+(i*13), 4));
		PlayerT* ply = GPlayers::GetPlayer(plyIndex);
		if(!ply){
			GPlayers::AddPlayer(plyIndex, "Unknown");
		}
		GPlayers::SetPlayerPos(plyIndex, x, y, dir);
	}
}

void GetNewPlayer(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("05")!=0) return;
	int index = atoi(eMsg.substr(2, 1).c_str());
	std::string nPlyName = eMsg.substr(3);
	GPlayers::AddPlayer(index, nPlyName);
	std::string message;
	message = "Player ";
	message += nPlyName;
	message += " has joined the game.";
	PlayerT* ply = GPlayers::GetPlayer(index);
	sortedPlyList.push_back(ply);
	AddChatMessage(message);
}

void ReceivePing(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("08")!=0) return;
	unsigned short index = SrlT::SetShort(eMsg.substr(2,2));
	if(index!=ClientSettings::GetMyIndex()){
		return;
	}
	char nBuf[3];
	itoa(ClientSettings::GetMyIndex(), nBuf, 10);
	std::string ePong = SrlT::GetShort(9);
	ePong += nBuf;
	GNetworking::SendToServer(ePong);
}

void ReceivePlayerKicked(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("10")!=0) return;
	int index = atoi(eMsg.substr(2,1).c_str());
	PlayerT* ply = GPlayers::GetPlayer(index);
	if(!ply) return;
	std::string newMsg = ply->name;
	newMsg += " timed out from the server.";
	AddChatMessage(newMsg);
	sortedPlyList.remove(ply);
	GPlayers::RemovePlayer(index);
}

bool CompareScore(PlayerT* a, PlayerT* b){
	if(a->score > b->score){
		return true;
	}
	if(a->score == b->score){
		if(a->name[0] < b->name[0]){
			return true;
		}
	}
	return false;
}

void ReceivePlayerWin(const std::string& eMsg){
	//if(eMsg.substr(0,2).compare("11")!=0) return;
	int plyID = atoi(eMsg.substr(2,1).c_str());
	PlayerT* ply = GPlayers::GetPlayer(plyID);

	unsigned int newMazeSeed = SrlT::SetInt(eMsg.substr(3, 4));
	unsigned short newMazeWidth = SrlT::SetShort(eMsg.substr(7, 2));
	unsigned short newMazeHeight = SrlT::SetShort(eMsg.substr(9, 2));

	ServerSettings::SetMazeSeed(newMazeSeed);
	ServerSettings::SetMazeWidth(newMazeWidth);
	ServerSettings::SetMazeHeight(newMazeHeight);

	SDL_Event nEvent;
	nEvent.type = SDL_USEREVENT;
	nEvent.user.data1 = (void*)5;
	SDL_PushEvent(&nEvent);

	std::string message = ply->name;
	ply->score++;
	message += " has won this level!";
	AddChatMessage(message);
	sortedPlyList.sort(CompareScore);
}

void MainGame::CreateMaze(int x, int y){
	PlayX=1;
	PlayY=1;
	if(Multiplayer){
		Maze::GenerateMaze(x, y, ServerSettings::GetMazeSeed());
	} else {
		Maze::GenerateMaze(x, y);
	}
	if(MazeModel.FloorModel){
		delete MazeModel.FloorModel;
		delete MazeModel.MazeModel;
		delete MazeModel.CeilingModel;
	}
	int fBlockX = Maze::GetMazeWidth()-1;
	int fBlockY = Maze::GetMazeHeight()-2;
	Block* finishBlock = Maze::GetBlock(fBlockX, fBlockY);
	finishBlock->SetBlockType(BLOCK_FINISH);
	MazeModel = Maze::CreateGeometry();
	if(MazeModelInst){
		MazeModelInst->model = MazeModel.MazeModel;
		FloorMdlInst->model = MazeModel.FloorModel;
		CeilingMdlInst->model = MazeModel.CeilingModel;
	}
	MazeScene->RemoveAllLights();
	int nW = int(floor(float(x)/2.0f));
	int nH = int(floor(float(y)/2.0f));
	for(int nY = 0; nY<nH; nY++){
		for(int nX = 0; nX<nW; nX++){
			Light* nLight = new Light();
			nLight->intensity = Colour(100, 100, 100);
			nLight->lPos = Vec3(float(nX)*2.0f+1.5f, float(nY)*2.0f+1.5f, 0.8f);
			nLight->lType = LIGHT_OMNI;
			nLight->shiny = 20.0f;
			nLight->strength = 5.0f;
			MazeScene->AddLight(nLight);
		}
	
	}
}

void ChatBoxEvent::SubmitMessage(Gwen::Controls::Base* control){
	std::string chatMsg = chatBox->GetText().Get();
	chatBox->Hide();
	if(chatMsg.size()<=0) return;
	std::string networkChat = SrlT::GetShort(6);
	char nBuf[3];
	itoa(ClientSettings::GetMyIndex(), nBuf, 10);
	networkChat += nBuf;
	networkChat += chatMsg;
	GNetworking::SendToServer(networkChat);
}

void MainGame::Event(SDL_Event &kevent){
	if(kevent.type == SDL_KEYUP && kevent.key.keysym.sym == SDLK_ESCAPE){
		Modules::SetFocus(activeMenu);
	}
	if(kevent.type == SDL_KEYUP && kevent.key.keysym.sym == SDLK_TAB){
		drawLeaderboard = false;
	}
	if(kevent.type == SDL_KEYDOWN && kevent.key.keysym.sym == SDLK_TAB && Multiplayer){
		drawLeaderboard = true;
	}
	if(kevent.type == SDL_MOUSEMOTION){
		int xDiff = kevent.motion.xrel;
		playDir+=float(xDiff)*GSettings::GetSensitivity();
		if(playDir>360.0f){
			playDir -= 360.0f;
		}
		if(playDir<0.0f){
			playDir += 360.0f;
		}

		int yDiff = kevent.motion.yrel;
		playYDir += float(yDiff)*GSettings::GetSensitivity();
		if(playYDir>90.0f){
			playYDir = 90.0f;
		}
		if(playYDir<-90.0f){
			playYDir = -90.0f;
		}
	}
	if(kevent.type == SDL_USEREVENT){
		int type = (int)kevent.user.data1;
		if(type==5){
			playX = 1.5f;
			playY = 1.5f;
			CreateMaze(ServerSettings::GetMazeWidth(), ServerSettings::GetMazeHeight());
			int fBlockX = Maze::GetMazeWidth()-1;
			int fBlockY = Maze::GetMazeHeight()-2;
			FinishTrophy->Position = Vec3(float(fBlockX)+0.5f, float(fBlockY)+0.5f, 0.0f);
		}
	}
	if(kevent.type == SDL_KEYUP && kevent.key.keysym.sym == SDLK_y){
		if(chatBox->Hidden()){
			chatBox->SetText("");
			chatBox->Show();
			chatBox->Focus();
		}
	}
}


void MainGame::BGEvent(SDL_Event &kevent){

}

void MainGame::Select(IModule* oldm){
	//SDL_ShowCursor(0);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void MainGame::DeSelect(IModule* newm){
	//SDL_ShowCursor(1);
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void MainGame::SetActiveMenu(BaseModule* newMenu){
	activeMenu=newMenu;
}

void MainGame::CreateResources(){
	if(!nFont){
		nFont = GraphicsM::GetFont("OpenSans.ttf", 11);
	}
	if(!chatBox && Multiplayer){
		chatBox = new Gwen::Controls::TextBox(GuiM::GetMainCanvas());
		chatBox->SetBounds(50, GraphicsM::ScrH() - 100, 400, 25);
		chatBox->onReturnPressed.Add(chatBox, &ChatBoxEvent::SubmitMessage);
		chatBox->Hide();
	}

	MazeScene = new Scene();
	int maWidth = 21;
	int maHeight = 21;
	if(Multiplayer){
		maWidth = ServerSettings::GetMazeWidth();
		maHeight = ServerSettings::GetMazeHeight();
	}
	CreateMaze(maWidth, maHeight);

	int fBlockX = Maze::GetMazeWidth()-1;
	int fBlockY = Maze::GetMazeHeight()-2;
	Block* finishBlock = Maze::GetBlock(fBlockX, fBlockY);
	

	CreatePerspectiveMatrix();
	//CreateIsometricMatrix();
	
	MTexture* floorTexture = new MTexture("floor.bmp");
	MTexture* ceilingTexture = new MTexture("ceiling.bmp");
	MTexture* wallTexture = new MTexture("wall.bmp");
	MTexture* ExitTexture = new MTexture("exit.bmp");
	Model* ExitModel = new Model("exit.obj");

	MTexture* playerTexture = new MTexture("player.bmp");
	playerModel = new Model("player.obj");
	playerMaterial = new Material();
	playerMaterial->ambient = Colour(10,10,10);
	playerMaterial->diffuse = Colour(255, 255, 255);
	playerMaterial->specular = Colour(100, 100, 100);
	playerMaterial->matTexture = playerTexture;

	Material* MazeMat = new Material();
	MazeMat->ambient = Colour(0, 0, 0);
	MazeMat->diffuse = Colour(255, 255, 255);
	MazeMat->specular = Colour(20, 20, 20);
	MazeMat->matTexture = wallTexture;

	Material* FloorMat = new Material();
	FloorMat->ambient = Colour(0, 0, 0);
	FloorMat->diffuse = Colour(255, 255, 255);
	FloorMat->specular = Colour(255, 255, 255);
	FloorMat->matTexture = floorTexture;

	MazeModelInst = new ModelInstance();
	MazeModelInst->Position = Vec3(0.0f, 0.0f, 0.0f);
	MazeModelInst->material = MazeMat;
	MazeModelInst->Scale = Vec3(1.0f, 1.0f, 1.0f);
	MazeModelInst->model = MazeModel.MazeModel;

	FloorMdlInst = new ModelInstance();
	FloorMdlInst->Position = Vec3(0.0f, 0.0f, 0.0f);
	FloorMdlInst->material = FloorMat;
	FloorMdlInst->Scale = Vec3(1.0f, 1.0f, 1.0f);
	FloorMdlInst->model = MazeModel.FloorModel;

	Material* CeilingMat = new Material();
	CeilingMat->ambient = Colour(0, 0, 0);
	CeilingMat->diffuse = Colour(255, 255, 255);
	CeilingMat->specular = Colour(100, 100, 100);
	CeilingMat->matTexture = ceilingTexture;

	CeilingMdlInst = new ModelInstance();
	CeilingMdlInst->Position = Vec3(0.0f, 0.0f, 0.0f);
	CeilingMdlInst->material = CeilingMat;
	CeilingMdlInst->Scale = Vec3(1.0f, 1.0f, 1.0f);
	CeilingMdlInst->model = MazeModel.CeilingModel;

	Material* finishMaterial = new Material();
	finishMaterial->ambient = Colour(100, 100, 100);
	finishMaterial->diffuse = Colour(255, 255, 255);
	finishMaterial->specular = Colour(255, 255, 255);
	finishMaterial->matTexture = ExitTexture;
	
	FinishTrophy = new ModelInstance();
	FinishTrophy->Position = Vec3(float(fBlockX)+0.5f, float(fBlockY)+0.5f, 0.0f);
	//FinishTrophy->Position = Vec3(0.0f, 0.0f, 0.0f);
	FinishTrophy->Scale = Vec3(1.0f, 1.0f, 1.0f);
	//Quaternion fAng;
	//fAng.x=1.0f;
	//fAng.y=1.0f;
	//fAng.z=0.0f;
	//fAng.w=0.0f;
	//FinishTrophy->Angle = fAng;
	FinishTrophy->material = finishMaterial;
	FinishTrophy->model = ExitModel;

	MazeScene->AddModel(MazeModelInst);
	MazeScene->AddModel(FloorMdlInst);
	MazeScene->AddModel(CeilingMdlInst);
	MazeScene->AddModel(FinishTrophy);


	MTexture* partShader = new MTexture("particle.png");
	partEmitter = new GParticles::TParticleEmitter();
	partEmitter->SetParticleTexture(partShader);

	printf("Resources Created\n");
}

MainGame::MainGame(){
	Multiplayer = false;
	CreateResources();

}

MainGame::MainGame(bool multi){
	Multiplayer = multi;
	CreateResources();
	if(multi){
		GNetworking::AddDecoder(NDecoder(&UpdatePositionData, 3));
		GNetworking::AddDecoder(NDecoder(&GetNewPlayer, 5));
		GNetworking::AddDecoder(NDecoder(&ReceiveCChat, 7));
		GNetworking::AddDecoder(NDecoder(&ReceivePing, 8));
		GNetworking::AddDecoder(NDecoder(&ReceivePlayerKicked, 10));
		GNetworking::AddDecoder(NDecoder(&ReceivePlayerWin, 11));

		for(int i=0; i<MAX_PLAYERS; i++){
			PlayerT* ply = GPlayers::GetPlayer(i);
			if(i==ClientSettings::GetMyIndex()) continue;
			if(!ply) continue;
			sortedPlyList.push_back(ply);
		}
		sortedPlyList.sort(CompareScore);
	}
}
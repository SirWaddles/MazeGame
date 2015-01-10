#include "server/srv_struct.h"

PlayerT* gPlayers[MAX_PLAYERS] = {NULL};

PlayerT::PlayerT(){
	x=0.0f;
	y=0.0f;
	lastPong=0;
	pingTime=0;
	dir=0.0f;
	index=-1;
	score = 0;
	active = false;
}

int GPlayers::AddPlayer(std::string name){
	int index = -1;
	for(int i=0; i<MAX_PLAYERS; i++){
		if(!gPlayers[i]){
			index = i;
			break;
		}
	}
	if(index==-1) return -1;
	PlayerT* newPly = new PlayerT();
	gPlayers[index] = newPly;
	newPly->index = index;
	newPly->name = name;

	//printf("wat\n");
	return index;
}

void GPlayers::AddPlayer(int index, std::string name){
	if(index>MAX_PLAYERS) return;
	if(index<0) return;
	if(gPlayers[index]){
		delete gPlayers[index];
		gPlayers[index] = NULL;
	}
	PlayerT* newPly = new PlayerT();
	gPlayers[index] = newPly;
	newPly->index = index;
	newPly->name = name;

}

bool GPlayers::IsPlayer(int index){
	if(index>=MAX_PLAYERS) return false;
	if(index<0) return false;
	if(gPlayers[index]){
		return true;
	} else return false;
}

void GPlayers::RemovePlayer(int index){
	PlayerT* ply = GPlayers::GetPlayer(index);
	if(!ply) return;
	delete ply;
	gPlayers[index] = NULL;
}

PlayerT* GPlayers::GetPlayer(int index){
	if(GPlayers::IsPlayer(index)){
		return gPlayers[index];
	}
	return NULL;
}

void GPlayers::SetPlayerPos(int index, float x, float y, float dir){
	PlayerT* play = GPlayers::GetPlayer(index);
	if(!play) return;
	play->x=x;
	play->y=y;
	play->dir = dir;
}

unsigned int mazeSeed=0;
unsigned short mazeWidth=0;
unsigned short mazeHeight=0;

unsigned int ServerSettings::GetMazeSeed(){
	return mazeSeed;
}

void ServerSettings::SetMazeSeed(unsigned int s){
	mazeSeed = s;
}

unsigned short ServerSettings::GetMazeWidth(){
	return mazeWidth;
}

unsigned short ServerSettings::GetMazeHeight(){
	return mazeHeight;
}

void ServerSettings::SetMazeWidth(unsigned short x){
	mazeWidth=x;
}

void ServerSettings::SetMazeHeight(unsigned short y){
	mazeHeight=y;
}

int myIndex = 0;

void ClientSettings::SetMyIndex(int index){
	myIndex = index;
}

int ClientSettings::GetMyIndex(){
	return myIndex;
}
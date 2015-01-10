#ifndef _SRV_STRUCT_HEADER_
#define _SRV_STRUCT_HEADER_

#include <string>

#define MAX_PLAYERS 6


struct PlayerT {
	bool active;
	float x;
	float y;
	float dir;
	std::string name;
	int index;
	int score;
	unsigned int lastPong;
	unsigned int pingTime;
	unsigned int joinTime;
	PlayerT();
};

namespace GPlayers{
	int AddPlayer(std::string name);
	void AddPlayer(int index, std::string name);
	void RemovePlayer(int index);
	bool IsPlayer(int index);
	PlayerT* GetPlayer(int index);
	void SetPlayerPos(int index, float x, float y, float dir=0.0f);
};

namespace ServerSettings {
	unsigned int GetMazeSeed();
	void SetMazeSeed(unsigned int s);
	unsigned short GetMazeWidth();
	unsigned short GetMazeHeight();
	void SetMazeWidth(unsigned short x);
	void SetMazeHeight(unsigned short y);
}

namespace ClientSettings {
	int GetMyIndex();
	void SetMyIndex(int index);
}



#endif
#ifndef _NETWORKA_HEADER_
#define _NETWORKA_HEADER_

#include <string>
#include "server/srv_struct.h"

struct NDecoder {
	int id;
	void (*sDecoder)(const std::string&);
	NDecoder(void (*sde)(const std::string&), int tId){
		sDecoder=sde;
		id=tId;
	}
	NDecoder() {
		sDecoder=NULL;
		id=0;
	}
};

namespace GNetworking {
	void InitNetworking();
	void DestroyNetworking();
	bool IsNetworking();
	void PollEvents();
	void AddDecoder(NDecoder decode);
	void RemoveDecoder(int decodeID);
	void SendData(PlayerT* play, std::string eMsg);
	void SendDataAll(std::string eMsg);
	void ConnectToServer(std::string ipAddy, int port);
	void SendToServer(std::string eMsg);
	void StartListening();
}





#endif // _NETWORK_HEADER_
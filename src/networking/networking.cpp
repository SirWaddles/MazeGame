#include "networking/networking.h"
#include "networking/networking_b.h"
#include "server/srv_struct.h"
#include "util/srl_types.h"
#include <boost/thread.hpp>

#include <list>
#include <deque>

#ifdef _M_SERVER_
#include "util/time.h"
#else
#include <SDL_timer.h>
#endif
using boost::asio::ip::udp;

//std::list<NDecoder> gDecoders;
NDecoder gDecoders[NET_DECODERS];

struct MsgSend {
	udp::endpoint toSend;
	std::string eMsg;
};

boost::mutex NThreadMutex;
std::deque<MsgSend> MsgsToSend;
std::deque<std::string> MsgsReceived;

void UDPMan::start_receive(){
	mSocket.async_receive_from(boost::asio::buffer(mRecvBuffer), mREndpoint, boost::bind(&UDPMan::handle_receive, 
		this, 
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred));
}

void UDPMan::handle_receive(const boost::system::error_code& error, std::size_t len){
	if(error){
		printf("Error: %s\n", error.message().c_str());
		return;
	}


	std::string eMsg(mRecvBuffer.data(), len);
	
	//printf("Message: %s\n", eMsg.c_str());

	NThreadMutex.lock();
	MsgsReceived.push_back(eMsg);
	NThreadMutex.unlock();

	start_receive();
}

void UDPMan::handle_send(boost::shared_ptr<std::string> message, const boost::system::error_code& error, std::size_t len){

}

void UDPMan::start_send(std::string eMsg, udp::endpoint remEnd){
	//printf("Sending Message: %s\n", eMsg.c_str());
	boost::shared_ptr<std::string> msgTS (new std::string(eMsg));
	mSocket.async_send_to(boost::asio::buffer(*msgTS), remEnd, 
		boost::bind(&UDPMan::handle_send, this, msgTS, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

udp::endpoint UDPMan::GetEndpoint(){
	return mREndpoint;
}

#ifdef _M_SERVER_
UDPMan::UDPMan(boost::asio::io_service& io_service) : mSocket(io_service, udp::endpoint(udp::v4(), 13)){
	mService = &io_service;
	//start_receive();
}
#else
UDPMan::UDPMan(boost::asio::io_service& io_service) : mSocket(io_service){
	mSocket.open(udp::v4());
	mService = &io_service;
}
#endif

udp::endpoint UDPMan::GetEndpoint(std::string ipaddy, std::string port){
	try {
		printf("Resolving %s:%s\n", ipaddy.c_str(), port.c_str());
		NThreadMutex.lock();
		udp::resolver resolver(*mService);
		udp::resolver::query query(udp::v4(), ipaddy, port);
		udp::endpoint resPoint = *resolver.resolve(query);
		NThreadMutex.unlock();
		return resPoint;
		
	} catch (std::exception &e){
		printf("Error: %s\n", e.what());
	}
	return udp::endpoint();
}

UDPMan* gNManager = NULL;
boost::asio::io_service* gMService = NULL;

void GNetworking::AddDecoder(NDecoder decode){
	gDecoders[decode.id]=decode;
	//gDecoders.push_back(decode);
}

void GNetworking::RemoveDecoder(int decodeID){
	gDecoders[decodeID]=NDecoder();
}

udp::endpoint playerPoints[MAX_PLAYERS];

void StartupPlayer(const std::string& eMsg){
	std::string playName = eMsg.substr(2, 20);
	int index = GPlayers::AddPlayer(playName);
	if(index<0){
		return;
	}
	PlayerT* ply = GPlayers::GetPlayer(index);
	playerPoints[index] = gNManager->GetEndpoint();
	std::string toSend = SrlT::GetShort(4);
	toSend += SrlT::GetShort(index);
	GNetworking::SendData(GPlayers::GetPlayer(index), toSend);
#ifdef _M_SERVER_
	ply->joinTime = GTime::GetTimeMs();
#endif
}

void InitPlayer(const std::string& eMsg){
	//if(eMsg.substr(0, 2).compare("00")!=0) return;
	unsigned int index = SrlT::SetShort(eMsg.substr(2,2));
	PlayerT* plyo = GPlayers::GetPlayer(index);
	if(!plyo) return;
	plyo->active = true;

	char* nChar = new char[5];
	itoa(index, nChar, 10);
	std::string toSend = SrlT::GetShort(1);
	toSend += nChar;
	toSend += SrlT::GetInt(ServerSettings::GetMazeSeed());
	toSend += SrlT::GetShort(ServerSettings::GetMazeWidth());
	toSend += SrlT::GetShort(ServerSettings::GetMazeHeight());
	for(int i=0; i<MAX_PLAYERS; i++){
		PlayerT* ply = GPlayers::GetPlayer(i);
		printf("ID: %i\n", i);
		if(!ply) continue;
		printf("PlayerName: %s\n", ply->name.c_str());
		if(i==index) continue;
		char nBuf[3];
		itoa(i, nBuf, 10);
		toSend += nBuf;
		toSend += SrlT::GetShort(ply->score);
		toSend += ply->name;
		toSend += "*";
	}
	GNetworking::SendData(plyo, toSend);
	std::string tellUsers = SrlT::GetShort(5);
	tellUsers += nChar;
	delete[] nChar;
	tellUsers += plyo->name;
	GNetworking::SendDataAll(tellUsers);
	printf("Player %s has joined the game with index %i\n", plyo->name.c_str(), index);
}

void AddToSendQueue(std::string eMsg, udp::endpoint point){
	MsgSend toSend;
	toSend.eMsg = eMsg;
	toSend.toSend = point;

	NThreadMutex.lock();
	MsgsToSend.push_back(toSend);
	NThreadMutex.unlock();
}

void GNetworking::SendData(PlayerT* play, std::string eMsg){
	if(!play){
		return;
	}
	//gNManager->start_send(eMsg, playerPoints[play->index]);
	//printf("Trying to send: %s\n", eMsg.c_str());
	AddToSendQueue(eMsg, playerPoints[play->index]);
}

void GNetworking::SendDataAll(std::string eMsg){
	for(int i=0; i<MAX_PLAYERS; i++){
		PlayerT* ply = GPlayers::GetPlayer(i);
		if(!ply) continue;
		GNetworking::SendData(ply, eMsg);
	}
}

boost::thread* procThread = NULL;
bool toListen = false;
bool closeThread = false;

void GNetworking::DestroyNetworking(){
	NThreadMutex.lock();
	closeThread=true;
	NThreadMutex.unlock();
	procThread->join();
	delete procThread;
	delete gNManager;
	delete gMService;
}

bool GNetworking::IsNetworking(){
	if(gNManager) return true; else return false;
}

void ProcessCalling(){
	bool quit = false;
	while(!quit){
		NThreadMutex.lock();
		//printf("Messages to Send: %i\n", MsgsToSend.size());
		while(!MsgsToSend.empty()){
			MsgSend toSend = MsgsToSend.front();
			gNManager->start_send(toSend.eMsg, toSend.toSend);
			MsgsToSend.pop_front();
		}
		if(toListen){
			gNManager->start_receive();
			toListen = false;
		}
		if(closeThread){
			quit=true;
		}
		NThreadMutex.unlock();
		gMService->poll();
#ifdef _M_SERVER_
		GTime::Delay(30);
#else
		SDL_Delay(30);
#endif
		//procThread->sleep(30);
	}
}

void GNetworking::InitNetworking(){
	if(!gNManager){
		gMService = new boost::asio::io_service;
		gNManager = new UDPMan(*gMService);
		procThread = new boost::thread(ProcessCalling);
#ifdef _M_SERVER_
		GNetworking::AddDecoder(NDecoder(&StartupPlayer, 0));
		GNetworking::AddDecoder(NDecoder(&InitPlayer, 12));
#endif
	}
}

void GNetworking::PollEvents(){
	std::list<std::string> currentMsgs;
	NThreadMutex.lock();
	while(!MsgsReceived.empty()){
		std::string toDecode = MsgsReceived.front();	
		currentMsgs.push_back(toDecode);
		MsgsReceived.pop_front();
	}
	NThreadMutex.unlock();
	std::list<NDecoder>::iterator it;
	std::list<std::string>::iterator msgIt;
	for(msgIt=currentMsgs.begin(); msgIt!=currentMsgs.end(); msgIt++){
		std::string toDecode = (*msgIt);
		unsigned short decodeID = SrlT::SetShort(toDecode.substr(0,2));
		if(!gDecoders[decodeID].sDecoder){
			printf("Received invalid ID: %i\n", decodeID);
			continue;
		}
		gDecoders[decodeID].sDecoder(toDecode);
	}
}

udp::endpoint* servPoint = NULL;

void GNetworking::ConnectToServer(std::string ipAddy, int port){
	char* portStr = new char[6];
	itoa(port, portStr, 10);
	std::string portS = portStr;
	delete[] portStr;
	if(servPoint){
		delete servPoint;
		servPoint = NULL;
	}
	servPoint = new udp::endpoint();
	*servPoint = gNManager->GetEndpoint(ipAddy, portS);
}

void GNetworking::SendToServer(std::string eMsg){
	//gNManager->start_send(eMsg, *servPoint);
	AddToSendQueue(eMsg, *servPoint);
}

void GNetworking::StartListening(){
	NThreadMutex.lock();
	toListen = true;
	NThreadMutex.unlock();
}
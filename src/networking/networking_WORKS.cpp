#include "networking/networking.h"
#include "networking/networking_b.h"
#include "server/srv_struct.h"
#include "util/srl_types.h"
#include <boost/shared_ptr.hpp>

#include <list>
using boost::asio::ip::udp;

std::list<NDecoder> gDecoders;

void UDPMan::start_receive(){
	mSocket.async_receive_from(boost::asio::buffer(mRecvBuffer), mREndpoint, boost::bind(&UDPMan::handle_receive, 
		this, 
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred));
}

void UDPMan::handle_receive(const boost::system::error_code& error, std::size_t len){
	if(error){
		printf("ErrorRecv: %s\n", error.message().c_str());
		printf("Error Code: %i\n", error.value());
		printf("Error caused by: %s\n", mREndpoint.address().to_string().c_str());
		return;
	}


	std::string eMsg(mRecvBuffer.data(), len);
	
	//printf("Message: %s\n", eMsg.c_str());

	std::list<NDecoder>::iterator it;
	for(it=gDecoders.begin(); it!=gDecoders.end(); it++){
		it->sDecoder(eMsg);
	}
	start_receive();
}

void UDPMan::handle_send(boost::shared_ptr<std::string> message, const boost::system::error_code& error, std::size_t len){
	if(error){
		printf("ErrorSend: %s\n", error.message().c_str());
		return;
	}
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
	start_receive();
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
		udp::resolver resolver(*mService);
		udp::resolver::query query(udp::v4(), ipaddy, port);
		udp::endpoint resPoint = *resolver.resolve(query);
		return resPoint;
		
	} catch (std::exception &e){
		printf("Error: %s\n", e.what());
	}
}

UDPMan* gNManager = NULL;
boost::asio::io_service* gMService = NULL;

int decoderID = 0;

void GNetworking::AddDecoder(NDecoder decode){
	decode.id = decoderID;
	decoderID++;
	gDecoders.push_back(decode);
}

udp::endpoint playerPoints[MAX_PLAYERS];

void InitPlayer(const std::string& eMsg){
	if(eMsg.substr(0, 2).compare("00")==0){
		std::string playName = eMsg.substr(2);
		int index = GPlayers::AddPlayer(playName);
		if(index<0){
			// Max players.
			return;
		}

		playerPoints[index] = gNManager->GetEndpoint();

		char* nChar = new char[5];
		itoa(index, nChar, 10);
		std::string toSend = "01";
		toSend+=nChar;
		toSend+= SrlT::GetInt(ServerSettings::GetMazeSeed());
		for(int i=0; i<MAX_PLAYERS; i++){
			PlayerT* ply = GPlayers::GetPlayer(i);
			if(!ply) continue;
			if(i==index) continue;
			char nBuf[3];
			itoa(i, nBuf, 10);
			toSend += nBuf;
			toSend += ply->name;
			toSend += "*";
		}
		GNetworking::SendData(GPlayers::GetPlayer(index), toSend);
		std::string tellUsers = "05";
		tellUsers += nChar;
		delete[] nChar;
		tellUsers += playName;
		GNetworking::SendDataAll(tellUsers);
		printf("Player %s has joined the game with index %i\n", playName.c_str(), index);
	}
}

void GNetworking::SendData(PlayerT* play, std::string eMsg){
	if(!play){
		return;
	}
	gNManager->start_send(eMsg, playerPoints[play->index]);
}

void GNetworking::SendDataAll(std::string eMsg){
	for(int i=0; i<MAX_PLAYERS; i++){
		PlayerT* ply = GPlayers::GetPlayer(i);
		if(!ply) continue;
		GNetworking::SendData(ply, eMsg);
	}
}

void GNetworking::InitNetworking(){
	if(!gNManager){
		gMService = new boost::asio::io_service;
		gNManager = new UDPMan(*gMService);
#ifdef _M_SERVER_
		GNetworking::AddDecoder(NDecoder(&InitPlayer));
#endif
	}
}

void GNetworking::PollEvents(){
	gMService->poll();
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
	gNManager->start_send(eMsg, *servPoint);
}

void GNetworking::StartListening(){
	gNManager->start_receive();
}
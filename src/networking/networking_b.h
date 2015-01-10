#ifndef _NETWORK_B_HEADER_
#define _NETWORK_B_HEADER_


#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#define NET_DECODERS 15

class UDPMan {
public:
	UDPMan(boost::asio::io_service& io_service);
	boost::asio::ip::udp::endpoint GetEndpoint();
	void start_send(std::string eMsg, boost::asio::ip::udp::endpoint remEnd);
	void start_receive();
	//void doPoll();
	boost::asio::ip::udp::endpoint GetEndpoint(std::string address, std::string port);
private:

	
	void handle_receive(const boost::system::error_code& error, std::size_t len);
	void handle_send(boost::shared_ptr<std::string> message, const boost::system::error_code& error, std::size_t len);

	boost::asio::io_service* mService;
	boost::asio::ip::udp::socket mSocket;
	boost::asio::ip::udp::endpoint mREndpoint;
	boost::array<char, 128> mRecvBuffer;
};








#endif
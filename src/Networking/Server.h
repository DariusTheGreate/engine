#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <string_view>

#include <Core/ThreadPool.h>
#include <Networking/NetworkSynchronizer.h>
#include <Engine/Scene.h>

using boost::asio::ip::tcp;
using BoostContext = boost::asio::io_context; 
using socket_ptr = boost::shared_ptr<boost::asio::ip::tcp::socket>;

template<typename Func>
void boostSaveUse(Func&& f)
{
	try{
		f();
	}
	catch(std::exception& e){
		std::cout << "EXCEPTION: " << e.what() << "\n";
	}
}

void sendData(boost::asio::ip::tcp::socket& hueket, const std::string& data);
void connectToSocket(boost::asio::ip::tcp::socket& hueket, tcp::resolver& resolver, std::string_view ip, std::string_view port);
size_t readSocket(boost::asio::ip::tcp::socket& hueket, boost::asio::streambuf& buffer, char endSymbol = '@');

class ClientConnection
{
public:
	ClientConnection(size_t port) : clientPort(port), acceptor(context, tcp::endpoint(tcp::v4(), port)) {}
public:
	void process();

	void sync(NetworkSynchronizer& syncer, Scene& currScene);

private:
	size_t clientPort = 0;
	BoostContext context;
	tcp::acceptor acceptor;
};

class Server
{
public:
	Server(NetworkSynchronizer& netSync_in, Scene& sc) : netSync(netSync_in), currScene(sc), acceptor(context, tcp::endpoint(tcp::v4(), 25000)), workers(8) {}

	void listen();

private:	
    NetworkSynchronizer& netSync;
    Scene& currScene;
    ThreadPool workers;

    BoostContext context;
    tcp::acceptor acceptor;
    size_t connections = 0;
};

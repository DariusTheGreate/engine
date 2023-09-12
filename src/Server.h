#pragma once

#include <iostream>
#include <boost/asio.hpp>

#include <ThreadPool.h>

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

class ClientConnection
{
public:
	ClientConnection(size_t port) : clientPort(port), acceptor(context, tcp::endpoint(tcp::v4(), port)) {}
public:
	void process();

private:
	size_t clientPort = 0;
	BoostContext context;
	tcp::acceptor acceptor;
};

class Server
{
public:
	Server() : acceptor(context, tcp::endpoint(tcp::v4(), 25000)), workers(8) {}

	void listen(size_t bufferSize);

private:	
    BoostContext context;
    tcp::acceptor acceptor;

    size_t connections = 0;

    ThreadPool workers;
};

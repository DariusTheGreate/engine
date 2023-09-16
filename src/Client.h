#pragma once

#include <Server.h>
#include <Profiler.h>

#include <type_traits>
#include <string>
#include <iostream>

template<typename T>
concept IsString = std::is_same_v<T, std::string&&>;	

enum class ClientEvent
{
	ParseScene,
};
 
class Client
{
public:		
	Client() : socket(context), resolver(context), acceptor(context, tcp::endpoint(tcp::v4(), 26000)) {

	}

	void serverRegistration();

	std::string query(const std::string& port, const std::string& data_in); //requires IsPod<T>  

	void sync(const std::string& port, NetworkSynchronizer& syncer, Scene* currScene);

private:
	BoostContext context;
	BoostContext contextRecv;
	tcp::socket socket;
    tcp::resolver resolver;
    tcp::acceptor acceptor;

    Profiler<double> profile;
};

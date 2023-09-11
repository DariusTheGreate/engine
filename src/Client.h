#pragma once

#include <Server.h>

#include <type_traits>
#include <string>
#include <iostream>

template<typename T>
concept IsString = std::is_same_v<T, std::string&&>;	
 
class Client
{
public:		
	Client() : socket(context), resolver(context), acceptor(context, tcp::endpoint(tcp::v4(), 26000)) {

	}

	void serverRegistration()
	{
		try{
		    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));
		}
		catch(std::exception& e)
		{
			std::cout << "connection went wrong\n";
		}

	    //auto result = boost::asio::write(socket, boost::asio::buffer("register"));
	}

	template<typename T> 
	void query(std::string port, T&& data_in) //requires IsPod<T>  
	{
		boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	
		std::cout << "my address: " << clientEndpoint.port() << "\n";
	    boost::asio::connect(socket, resolver.resolve("127.0.0.1", port));
	    std::cout << "connected\n";
	    //T datacopy = data_in;
	    auto result = boost::asio::write(socket, boost::asio::buffer(std::forward<T>(data_in)));
	    std::cout << "written\n";
	    /*while(1){
	    	std::string datahere;
	    	std::cin >> datahere;
		    auto resultin = boost::asio::write(socket, boost::asio::buffer(std::move(datahere)));
		}
		*/

		/*if(datacopy == "scene")
		{
			acceptor.accept(socket);
            boost::asio::streambuf buffer;
            boost::system::error_code error;
            boost::asio::read(socket, buffer, boost::asio::transfer_exactly(5), error);//NOTE(darius) fkn boost blocks until u read at least n. So u needd msg to > than n eta pzdc suka

            if(buffer.size() > 0)
            {
            	std::cout << "scene here\n"; 
            }
		}*/

	    boost::system::error_code ec;
		//socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	    //socket.close();
	}

private:
	BoostContext context;
	BoostContext contextRecv;
	tcp::socket socket;
    tcp::resolver resolver;
    tcp::acceptor acceptor;
};

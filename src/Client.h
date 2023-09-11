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

	void serverRegistration();

	template<typename T> 
	void query(std::string port, T&& data_in) //requires IsPod<T>  
	{
		T datacopy = data_in;
		boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	

		std::cout << "my port before connect : " << clientEndpoint.port() << "\n";
		try{
		    boost::asio::connect(socket, resolver.resolve("127.0.0.1", port));//NOTE(darius) changes socket
			//std::cout << "my port after connect : " << clientEndpoint.port() << "\n";//same

		    std::cout << "connected\n";

		    auto result = boost::asio::write(socket, boost::asio::buffer(std::forward<T>(data_in)));
		}
		catch(std::exception& e){
			std::cout << "EXCEPTION: " << e.what() << "\n";
		}

		std::cout << "my port after write: " << clientEndpoint.port() << "\n";
	    std::cout << "written\n";
	    /*while(1){
	    	std::string datahere;
	    	std::cin >> datahere;
		    auto resultin = boost::asio::write(socket, boost::asio::buffer(std::move(datahere)));
		}
		*/

		if(datacopy == "scene@")
		{
			std::cout << "wait for scene\n";
			try{
				boost::asio::streambuf buffer;
	            boost::system::error_code error;
	            //boost::asio::read(socket, buffer, boost::asio::transfer_exactly(5), error);//NOTE(darius) fkn boost blocks until u read at least n. So u needd msg to > than n eta pzdc suka
	            size_t bytes_transferred = boost::asio::read_until(socket, buffer, '@');

	            if(bytes_transferred > 0){
		            std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
    	            std::cout << "res of read " << s;
		        }

			}
			catch(std::exception& e)
			{
				std::cout << "EXCEPTION: " << e.what() << "\n";
			}
			//acceptor.ccept(socket);
            //boost::asio::streambuf buffer;
            //boost::system::error_code error;
            //boost::asio::read(socket, buffer, boost::asio::transfer_exactly(5), error);//NOTE(darius) fkn boost blocks until u read at least n. So u needd msg to > than n eta pzdc suka

            //if(buffer.size() > 0)
            //{
            //	std::cout << "scene here\n"; 
            //}
		}

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

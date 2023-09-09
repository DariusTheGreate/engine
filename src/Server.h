#pragma once

#include <iostream>
#include <boost/asio.hpp>

#include <ThreadPool.h>

using boost::asio::ip::tcp;
using BoostContext = boost::asio::io_context; 

class Server
{
public:
	Server() : acceptor(context, tcp::endpoint(tcp::v4(), 25000)), workers(6) {}

	void listen(size_t bufferSize)
	{
		try
		{	
			while(true)
			{
	            tcp::socket socket(context);
	            std::cout << "waiting" << std::endl;
	            acceptor.accept(socket);

	            std::cout << "Accepted connection from " << socket.remote_endpoint() << std::endl;

	            boost::system::error_code error;
	            
	            boost::asio::streambuf buffer;
	            boost::asio::read(socket, buffer, boost::asio::transfer_exactly(bufferSize), error);

	            std::string data{
	                std::istreambuf_iterator<char>(&buffer), 
	                std::istreambuf_iterator<char>() 
	            };

	            if (error == boost::asio::error::eof)
	            {
	                std::cout << "Connection closed by peer" << std::endl;
	            }
	            else if (error)
	            {
	                throw boost::system::system_error(error);
	            }
	            else
	            {
    				workers.Submit([&data](){
		            	std::cout << std::this_thread::get_id() << "\n";
		                std::cout << "Received: " << data << std::endl;
		                //std::string response = "Hello, client!";
		                //boost::asio::write(socket, boost::asio::buffer(response));
		            });
    			}
			}		
		}
		catch(std::exception& e)
		{
	        std::cerr << "Exception: " << e.what() << std::endl;
		}
	}

private:	
    BoostContext context;
    tcp::acceptor acceptor;

    ThreadPool workers;
};

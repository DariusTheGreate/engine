#pragma once

#include <Server.h>

#include <type_traits>
#include <string>

template<typename T>
concept IsString = std::is_same_v<T, std::string&&>;	
 
class Client
{
public:		
	template<typename T> void query(T&& data) //requires IsPod<T>  
	{
		tcp::socket socket(context);
	    tcp::resolver resolver(context);

	    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));
	    
	    auto result = boost::asio::write(socket, boost::asio::buffer(std::forward<T>(data)));
	    
	    boost::system::error_code ec;

	    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	    socket.close();
	}
private:
	BoostContext context;
};

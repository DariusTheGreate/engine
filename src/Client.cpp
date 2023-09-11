#include <Client.h>

void Client::serverRegistration()
{
	try{
	    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

		boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	
		std::cout << "my port registration connect : " << clientEndpoint.port() << "\n";
	}
	catch(std::exception& e)
	{
		std::cout << "connection went wrong\n";
		std::cout << e.what() << "\n";
	}

    //auto result = boost::asio::write(socket, boost::asio::buffer("register"));
}

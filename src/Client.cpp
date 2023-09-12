#include <Client.h>
#include <Editor.h>

void Client::serverRegistration()
{
	boostSaveUse([this](){
	    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));
		boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	
		std::cout << "my port registration connect : " << clientEndpoint.port() << "\n";
	});

    //auto result = boost::asio::write(socket, boost::asio::buffer("register"));
}

std::string Client::query(const std::string& port, const std::string& data_in) //requires IsPod<T>  
{
	auto datacopy = data_in;
	boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	

	boostSaveUse([this, port, data_in](){
	    boost::asio::connect(socket, resolver.resolve("127.0.0.1", port));//NOTE(darius) changes socket
		//std::cout << "my port after connect : " << clientEndpoint.port() << "\n";//same
	    //auto result = boost::asio::write(socket, boost::asio::buffer(std::move(data_in)));
	    sendData(socket, data_in);
	});

	if(datacopy == "scene@")
	{
		//boostSaveUse([this](){
		try{
			boost::asio::streambuf buffer;
            boost::system::error_code error;
            size_t bytes_transferred = boost::asio::read_until(socket, buffer, '@');

            if(bytes_transferred > 0){
	            std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
	            return s;
	        }

		}
		catch(std::exception& e)
		{
			std::cout << "EXCEPTION: " << e.what() << "\n";
		}
	}

	//socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    //socket.close();

    return {};
}

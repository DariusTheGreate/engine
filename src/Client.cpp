#include <Client.h>
#include <Editor.h>
#include <sstream>
#include <Timer.h>

void Client::serverRegistration()
{
	boostSaveUse([this](){
	    connectToSocket(socket, resolver, "127.0.0.1", "25000");
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
	    connectToSocket(socket, resolver, "127.0.0.1", port);
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

void Client::sync(const std::string& port, NetworkSynchronizer& syncer, Scene* currScene)
{
	boost::asio::ip::tcp::endpoint clientEndpoint = socket.local_endpoint();	

	boostSaveUse([this, port, &syncer, &currScene](){
	    connectToSocket(socket, resolver, "127.0.0.1", port);
	    std::cout << "connected\n";

	    if(syncer.size()){
		    std::stringstream ss;
		    //TODO(darius) wrong other thread may change size or object
		    //TODO(darius) BUG(darius) if delete object - will crash all due to cring vector objects design.
		    std::cout << "sync sz: " << syncer.size() << "\n";
	    	for(int i = 0; i < syncer.size(); ++i)
	    	{
			    Object* obj = nullptr; 
			    syncer.TakeAt(obj, i);

			    if(obj){
				    std::cout << "serializing: " << i << "\n";
				    obj->serialize(ss);
			    }
	    	}

	    	Timer timeToSend;

		    ss << "@";
		    sendData(socket, ss.str());
		    profile.addRecord(timeToSend.checkTime());
		    //std::cout << "sended data, time to send: " << timeToSend.checkTime(); //NOTE(darius) less than 1ms localy, check what about distanced
		}

	    boost::asio::streambuf buffer;
        boost::system::error_code error;
        size_t bytes_transferred = 0;

		boostSaveUse([&](){ 
	        bytes_transferred = readSocket(socket, buffer);//boost::asio::read_until(*socket2, buffer, '@');
	    });        	

        if(bytes_transferred > 0){
            std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
            std::cout << "response: " << s << "\n";
        	currScene->parseSynchronizationMsg(s);
        }
	});
}

#include <Networking/Server.h>
#include <Engine/Editor.h>
#include <Core/NonBlockingQueue.h>
#include <Core/Timer.h>

void sendData(boost::asio::ip::tcp::socket& hueket, const std::string& data)
{
	boost::asio::write(hueket, boost::asio::buffer(data));
}

void connectToSocket(boost::asio::ip::tcp::socket& hueket, tcp::resolver& resolver, std::string_view ip, std::string_view port)
{
	boost::asio::connect(hueket, resolver.resolve(ip, port));
}

size_t readSocket(boost::asio::ip::tcp::socket& hueket, boost::asio::streambuf& buffer, char endSymbol)
{
	return boost::asio::read_until(hueket, buffer, endSymbol);
}

void ClientConnection::process()
{
	while(1)
	{
		socket_ptr socket2(new boost::asio::ip::tcp::socket(context));
        acceptor.accept(*socket2);

        boost::asio::streambuf buffer;
		std::cout << "Waiting for client message.." << std::endl;
        boost::system::error_code error;
        size_t bytes_transferred = boost::asio::read_until(*socket2, buffer, '@');

        if (bytes_transferred > 0){ 
        	std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
        	//std::cout << "ID: " << std::this_thread::get_id() << " ";
			std::cout << "MSG: " << s << "\n";

			if(s == "exit@")
			{
				break;
			}

			if(s == "scene@")
			{
				boost::asio::ip::tcp::endpoint clientEndpoint = socket2->remote_endpoint();	

				boostSaveUse([this, socket2](){
					std::string data = Editor::getScene()->readFileToString(GameState::engine_path + "sceneBatched.dean");
					data.append("@");

					std::cout << "sending scene...\n";
					sendData(*socket2, data);
				});
			}

			if(s == "message@")
			{
				boost::asio::ip::tcp::endpoint clientEndpoint = socket2->remote_endpoint();	

				std::string fknadres = clientEndpoint.address().to_string();//, clientEndpoint.address().to_string(), std::to_string(clientEndpoint.port())
				std::string fknport = std::to_string(clientEndpoint.port());

				std::cout << "address socket endpoint " << fknadres << " " << fknport << "\n";

				boostSaveUse([this, socket2](){
					while(1){
						std::string data;
						std::cout << "type msg: ";
						std::cin >> data;
						data.append("@");
						std::cout << data;
						sendData(*socket2, data);
					}
				});
			}
        }

        socket2->close();
    }
}

//TODO(darius) here currScene is &, but in client is *. Choose one
void ClientConnection::sync(NetworkSynchronizer& syncer, Scene& currScene)
{
	while(1)
	{
		socket_ptr socket2(new boost::asio::ip::tcp::socket(context));
        acceptor.accept(*socket2);

        boost::asio::streambuf buffer;
		std::cout << "Waiting for client message.." << std::endl;
        boost::system::error_code error;
        size_t bytes_transferred = 0;
        //NOTE(darius) blocks here
       	boostSaveUse([&](){ 
	        bytes_transferred = readSocket(*socket2, buffer);//boost::asio::read_until(*socket2, buffer, '@');
	    });

        if(bytes_transferred > 0)
        {
        	std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
        	//std::cout << s << "\n"; 	

        	if(std::strcmp(s.c_str(), "EMPTY_SYNCER@") != 0)
	        	currScene.parseSynchronizationMsg(s);

        	boostSaveUse([&](){
	        	std::stringstream ss;
			    //TODO(darius) wrong other thread may change size or object
			    //TODO(darius) BUG(darius) if delete object - will crash all due to cring vector objects design.
			    std::cout << "sync sz: " << syncer.size() << "\n";

			    //TODO(darius) refactor
			    if(!syncer.size()){
			    	ss << "EMPTY_SYNCER@";
				    sendData(*socket2, ss.str());
				    return;
			    }

		    	for(int i = 0; i < syncer.size(); ++i)
		    	{
				    Object* obj = nullptr; 
				    syncer.TakeAt(obj, i);

				    if(obj){
					    //std::cout << "serializing: " << i << "\n";
					    obj->serialize(ss);
				    }
		    	}

		    	Timer timeToSend;

			    ss << "@";
			    sendData(*socket2, ss.str());
			    //std::cout << "sended data, time to send: " << timeToSend.checkTime(); //NOTE(darius) less than 1ms localy, check what about distanced
        	});
        }
	}
}

void Server::listen()
{
	while(true)
	{
		if(connections > 8)
			continue;

        socket_ptr socket(new boost::asio::ip::tcp::socket(context));

        std::cout << "Waiting For Registration.." << std::endl;
        acceptor.accept(*socket);
        std::cout << "Accepted REGISTRATION from " << socket->remote_endpoint() << std::endl;

        size_t port = connections;
		workers.SubmitVoid([port, this](){
			ClientConnection conn(25001 + port);
			//conn.process();
			conn.sync(netSync, currScene);
        });

        connections++;
	}		
}

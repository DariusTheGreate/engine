#include <Server.h>
#include <Editor.h>

void sendData(boost::asio::ip::tcp::socket& hueket, const std::string& data)
{
	boost::asio::write(hueket, boost::asio::buffer(data));
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
        //boost::asio::read(*socket2, buffer, boost::asio::transfer_all(), error);//NOTE(darius) fkn boost blocks until u read at least n. So u needd msg to > than n eta pzdc suka
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
			    tcp::resolver resolver(context);
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
				tcp::resolver resolver(context);
				boost::asio::ip::tcp::endpoint clientEndpoint = socket2->remote_endpoint();	
				//socket_ptr clientSocket(new boost::asio::ip::tcp::socket(context));

				std::string fknadres = clientEndpoint.address().to_string();//, clientEndpoint.address().to_string(), std::to_string(clientEndpoint.port())
				std::string fknport = std::to_string(clientEndpoint.port());

				std::cout << "address socket endpoint " << fknadres << " " << fknport << "\n";

				boostSaveUse([this, socket2](){
				    //boost::asio::connect(*clientSocket, resolver.resolve(fknadres, fknport));
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

void Server::listen(size_t bufferSize)
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
		workers.SubmitVoid([port](){
			ClientConnection conn(25001 + port);
			std::cout << "created conn\n";
			conn.process();
        });

        connections++;
	}		
}

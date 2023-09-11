#include <Server.h>

void ClientConnection::process()
{
	while(1)
	{
		std::cout << "start loop\n";
		socket_ptr socket2(new boost::asio::ip::tcp::socket(context));
        acceptor.accept(*socket2);
        std::cout << "Accepted connection from " << socket2->remote_endpoint() << std::endl;

        boost::asio::streambuf buffer;
		std::cout << "Waiting for client message.." << std::endl;

        boost::system::error_code error;

        //boost::asio::read(*socket2, buffer, boost::asio::transfer_all(), error);//NOTE(darius) fkn boost blocks until u read at least n. So u needd msg to > than n eta pzdc suka
        size_t bytes_transferred = boost::asio::read_until(*socket2, buffer, '@');


        if (bytes_transferred > 0){ 
        	std::string s(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());

        	std::cout << "ID: " << std::this_thread::get_id() << " ";
			std::cout << "MSG: " << s << "\n";

			if(s == "exit@")
			{
				break;
			}

			//NOTE(darius) crashes
			if(s == "scene@")
			{
			    tcp::resolver resolver(context);
				boost::asio::ip::tcp::endpoint clientEndpoint = socket2->remote_endpoint();	
				//socket_ptr clientSocket(new boost::asio::ip::tcp::socket(context));

				std::string fknadres = clientEndpoint.address().to_string();//, clientEndpoint.address().to_string(), std::to_string(clientEndpoint.port())
				std::string fknport = std::to_string(clientEndpoint.port());

				std::cout << "address socket endpoint " << fknadres << " " << fknport << "\n";

				try{
					std::cout << "write to scoket\n";
				    //boost::asio::connect(*clientSocket, resolver.resolve(fknadres, fknport));
					boost::asio::write(*socket2, boost::asio::buffer("temp scene val@"));
				}
				catch(std::exception& e)
				{
					std::cout << "EXCEPTION: " << e.what() << "\n";
				}
			}

            //std::string response = "Hello, client!";
            //boost::asio::write(*socket, boost::asio::buffer(response));
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

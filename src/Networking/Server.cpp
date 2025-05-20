#define _WIN32_WINNT 0x0601
#include <Networking/Server.h>
#include <Engine/Editor.h>
#include <Core/NonBlockingQueue.h>
#include <Core/Timer.h>

#include <Networking/PriorityScheduler.h>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using namespace boost::interprocess;

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
	socket_ptr socket2(new boost::asio::ip::tcp::socket(context));
    acceptor.accept(*socket2);
	while(1)
	{
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

        	if(std::strcmp(s.c_str(), "REGISTER@") == 0)
        	{
        		continue;
        	}

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
	PriorityScheduler sched;
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

// Shared memory structure
struct SharedData {
	interprocess_mutex mutex;
	interprocess_condition cond_var;
	bool has_data = false;
	std::array<uint8_t, 1024> data;

	static void createSharedData() 
	{
		managed_shared_memory managed_shm{ open_or_create, "BinanceSharedMemory", 1024 };
		SharedData* shared_data = managed_shm.construct<SharedData>("SharedData")();
	}

	static void readSharedData()
	{
		// Open existing managed shared memory
		managed_shared_memory managed_shm{ open_only, "BinanceSharedMemory" };

		// Find the shared data structure in shared memory
		SharedData* shared_data = managed_shm.find<SharedData>("SharedData").first;

		if (!shared_data) {
			std::cerr << "Consumer: Shared data not found!\n";
			return;
		}

		scoped_lock<interprocess_mutex> lock(shared_data->mutex);

		shared_data->cond_var.wait(lock, [&shared_data] { return shared_data->has_data; });

		// do stuff with data
	}
};


void Server::createSharedMemory() 
{
	boost::interprocess::shared_memory_object shm_obj(boost::interprocess::create_only, "shared_memory", boost::interprocess::read_write);
	shm_obj.truncate(10000);
}


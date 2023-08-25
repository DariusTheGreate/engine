#include <iostream>
#include <signal.h>     // ::signal, ::raise
#include <boost/stacktrace.hpp>

#include "Application.h"
#include <UI.h>
#include "GameState.h"

Camera GameState::cam;
MouseState GameState::ms;
//KeyboardState GameState::ks;

void setSignalHandlers(int signum)
{
	::signal(signum, SIG_DFL);
    std::cout << boost::stacktrace::stacktrace();
    std::string path = GameState::engine_path + "backtrace.dump";
	//boost::stacktrace::safe_dump_to(path.c_str());
    //TODO(darius) make it filesystem at some point
    std::ofstream out;
    out.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
    out << boost::stacktrace::stacktrace();
    out.close();
	::raise(SIGABRT);
}

int main()
{
	std::cout << "------\n";
    //TODO(darius) make it own class. Allow multithreading
    ::signal(SIGSEGV, &setSignalHandlers);
    ::signal(SIGABRT, &setSignalHandlers);

    /*if (boost::filesystem::exists(GameState::engine_path + "backtrace.dump")) {
        // there is a backtrace
        std::ifstream ifs(GameState::engine_path + "backtrace.dump");

        boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(ifs);
        //std::cout << "Previous run crashed:\n" << st << std::endl;

        // cleaning up
        //ifs.close();
        //boost::filesystem::remove(GameState::engine_path + "backtrace.dump");
    }
    */

    try {
        Application* app = new Application((char*)"Dean", 1920, 1080);
        app->Run();
        std::cout << "------\n";
    }
    catch (...) 
    {
        std::terminate();//NOTE(darius) will save stacktrace, but i dont like it
    }
    return 0;
}


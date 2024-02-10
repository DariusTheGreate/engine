#include <windows.h>
#include <iostream>

#include "Engine/Application.h"
#include <Engine/UI.h>
#include "Engine/GameState.h"
#include <Engine/StackTrace.h>
#include <Core/ThreadPool.h>

#define DEAN_UNIT_TEST_SHIT

#ifdef DEAN_UNIT_TEST_SHIT
    #include <Engine/UnitTests.h>
#endif

int main()
{
    try {
        std::cout << "------\n";
        StackTrace backtrace;

        #ifdef DEAN_UNIT_TEST_SHIT
            println("Runing tests..");
            UnitTests::run();
            println("Tests Passed!");
        #endif

        Application* app = new Application((char*)"Dean", 1920, 1080);
        app->Run();

        std::cout << "------\n";
    }
    catch(std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        throw std::exception();
    }
    catch (...) 
    {
        std::terminate();//NOTE(darius) will save stacktrace, but i dont like it
    }

    return 0;
}

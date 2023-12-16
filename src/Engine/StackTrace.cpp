#include <Engine/StackTrace.h>
#include <Engine/GameState.h>

#include <fstream>

StackTrace::StackTrace()
{
    ::signal(SIGSEGV, &StackTrace::setSignalHandlers);
    ::signal(SIGABRT, &StackTrace::setSignalHandlers);

    StackTrace::initialized = true;
}

void StackTrace::setSignalHandlers(int signum)
{
    //if(!StackTrace::initialized)
    //    return;

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

void StackTrace::printCurrentStackTrace()
{
    if(!StackTrace::initialized)
        return;

    std::cout << boost::stacktrace::stacktrace() << "\n";
}

void StackTrace::printFunctionName(boost_stacktrace_function_t func)
{
    if(!StackTrace::initialized)
        return;
    
    boost::stacktrace::frame f(func);
    std::cout << f << std::endl;
}

bool StackTrace::initialized = false;

#pragma once

#include <iostream>
#include <signal.h>     // ::signal, ::raise
#include <boost/stacktrace.hpp>
#include <boost/exception/error_info.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/enable_error_info.hpp>

using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

template <class E>
void throw_with_trace(const E& e) {
    throw boost::enable_error_info(e)
        << traced(boost::stacktrace::stacktrace());
}

typedef void(*boost_stacktrace_function_t)(int);

class StackTrace
{

public:
    StackTrace();

public:
    static void setSignalHandlers(int signum);

    static void printCurrentStackTrace();

    static void printFunctionName(boost_stacktrace_function_t func);

private:
    static bool initialized;
    
};


#include "../inc/currentThread.h"
#include "../inc/timestamp.h"

extern "C"{
#include <execinfo.h>
}

#include <string>
#include <cxxabi.h>
using namespace std;

namespace muduo{

namespace currentThread{
__thread pid_t t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "unknown";

void cache_tid(){
    t_cachedTid = gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
}

pid_t tid(){
    if(t_cachedTid == 0){
        cache_tid();
    }
    return t_cachedTid;
}

const char* tid_string(){
    return t_tidString;
}

int tid_stringLength(){
    return t_tidStringLength;
}
const char* name(){
    return t_threadName;
}

bool is_main_thread(){
    return tid() == getpid();
}

void sleep_usec(int64_t usec){
    struct timespec ts = { 0, 0 };
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::sToUs);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::sToUs * 1000);
    ::nanosleep(&ts, NULL);
}

string stackTrace(bool demangle){
    const int max_frames = 200;
    void *frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char** strings = ::backtrace_symbols(frame, nptrs);
    string stack;
    if(strings){
        size_t len = 256;
        char* demangled = demangle ? reinterpret_cast<char*>(malloc(len)) : nullptr;
        for(int i = 0; i < nptrs; ++i){
            if(demangled){
                char* left_par = nullptr;
                char* plus = nullptr;
                for(char* p = strings[i]; *p; ++p){
                    if (*p == '(')
                        left_par = p;
                    else if(*p == '+')
                        plus = p;
                }
                if(left_par && plus){
                    *plus = '\0';
                    int status = 0;
                    char* ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
                    *plus = '+';
                    if(status == 0){
                        demangled = ret;  
                        stack.append(strings[i], left_par + 1);
                        stack.append(demangled);
                        stack.append(plus);
                        stack.push_back('\n');
                        
                        continue;
                    }
                }
            }
            
            stack.append(strings[i]);
            
            stack.push_back('\n');
        }
        free(demangled);
        free(strings);
    }
    return stack;
}

}


}
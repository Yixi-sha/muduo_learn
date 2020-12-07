#ifndef __MY_CURRENTTHREAD_H__
#define __MY_CURRENTTHREAD_H__

#include "noncopyable.h"

extern "C"{
#include <sys/types.h>
#include <unistd.h>
}

namespace muduo{

namespace currentThread{
extern __thread pid_t t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;

void cache_tid();
pid_t tid();
const char* tid_string();
int tid_stringLength();
const char* name();
bool is_main_thread();
void sleep_usec(int64_t usec);
string stackTrace(bool demangle);

}



}

#endif
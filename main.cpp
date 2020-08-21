#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

extern "C"{
#include <sys/timerfd.h>
}


#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/eventLoop.h"
#include "muduo/inc/timestamp.h"
#include "muduo/inc/poller.h"
#include "muduo/inc/channel.h"
#include "muduo/inc/timer.h"
#include "muduo/inc/eventLoopThread.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;


void timeout(){
    cout <<"timeout!!!!!!!!!!! "<< endl;
    cout << "timeout   " << pthread_self()<< endl;
    g_loop->quit();
}

int i = 9999;

void *thread_func(void *argv){
    cout << "thread_func   " << pthread_self()<< endl;
    //g_loop->run_after(1, timeout);
    return &i;
}

int main()
{

    cout << "yixi-test begin" << endl;
    EventLoop localLoop;
    g_loop = &localLoop;
    EventLoopThread eventLoopThread;
    EventLoop *otherLoop = eventLoopThread.start_loop();
    otherLoop->run_in_loop(bind(thread_func, nullptr));
    cout << "main   " << pthread_self()<< endl;
    //localLoop.loop();
    sleep(1);
    otherLoop->run_in_loop(bind(thread_func, nullptr));
    sleep(3);
    otherLoop->quit();
    sleep(3);
    cout << "yixi-test end" << endl;

    return 0;
}

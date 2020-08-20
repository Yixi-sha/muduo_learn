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

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;


void timeout(){
    cout <<"timeout!!!!!!!!!!! "<< endl;

}

void *thread_func(void *){
    cout << "thread_func   " << pthread_self()<< endl;
    g_loop->run_after(1, timeout);
    return nullptr;
}

int main()
{

    cout << "yixi-test begin" << endl;
    EventLoop localLoop;
    g_loop = &localLoop;
    Thread thread(thread_func);
    thread.start();
    localLoop.loop();
    thread.join(nullptr);
    cout << "yixi-test end" << endl;

    return 0;
}

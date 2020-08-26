#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>


extern "C"{
#include <sys/timerfd.h>
#include <unistd.h>
#include <sys/eventfd.h>
}

#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/eventLoop.h"
#include "muduo/inc/timestamp.h"
#include "muduo/inc/poller.h"
#include "muduo/inc/channel.h"
#include "muduo/inc/timer.h"
#include "muduo/inc/eventLoopThread.h"
#include "muduo/inc/TcpServer.h"
#include "muduo/inc/acceptor.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;
void test(AcceptFd acceptFd){
    cout <<acceptFd.get_fd() << endl;
}
int main(){
    cout << "yixi-test begin" << endl;
    shared_ptr<EventLoop> eventLoop(EventLoop::construct_eventLoop());
     
    shared_ptr<Acceptor> acceptor(Acceptor::construct_accpetor("ubuntu","8888", eventLoop.get()));
    acceptor->set_new_connect_callback(test);
    acceptor->listen();
    eventLoop->loop();
    cout << eventLoop.get() << endl;
    cout << "yixi-test end" << endl;
    return 0;
}

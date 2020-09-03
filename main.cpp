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
#include "muduo/inc/TcpServer.h"
#include "muduo/inc/buffer.h"
#include "muduo/inc/connector.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;

void my_connect(int fd){
    cout << "connect  " <<fd << endl;
}

int main(){
    cout << "yixi-test begin" << endl; 
    shared_ptr<EventLoop> loop(EventLoop::construct_eventLoop());
    g_loop = loop.get();
    shared_ptr<Connector> connector(Connector::construct_connector("ubuntu", "8888", g_loop));
    connector->set_newConnectionCallback(my_connect);
    connector->start();
    loop->loop();
    cout << "yixi-test end" << endl;
    return 0;
}

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
#include "muduo/inc/TcpServer.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;


int main()
{
    cout << "yixi-test begin" << endl;
    shared_ptr<TcpServer> t(TcpServer::construct_tcpServer("0.0.0.0", "8888"));
    cout << t->get_fd() << endl; 
    
    cout << "yixi-test end" << endl;

    return 0;
}

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

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;

void newConnection(const string name,const SocketAddr local,const SocketAddr peer, TcpConnectionFd::StateE state){
    cout << state << " newConnection "<<name << " " << local.toString() << "\t" << peer.toString() << endl;
}

void message(const string name,const SocketAddr local, SocketAddr peer,char *buf,const int len){
    buf[len] = '\0';
    cout << "read "<< buf << endl;
}

int main(){
    cout << "yixi-test begin" << endl; 
    shared_ptr<EventLoop> eventLoop(EventLoop::construct_eventLoop());
    shared_ptr<TcpServer> tcpServer(TcpServer::construct_TcpServer("ubuntu", "8888", eventLoop.get(),true));
    tcpServer->set_messageCallback(message);
    tcpServer->set_newConnectionCallback(newConnection);
    cout << tcpServer->name() << endl;
    tcpServer->start();
    eventLoop->loop();
    cout << "yixi-test end" << endl;
    return 0;
}

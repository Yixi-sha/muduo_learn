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

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;
TcpServer *g_tcpServer;
void connection_state(const string name, const SocketAddr, const SocketAddr, TcpConnectionFd::StateE state){
    cout << "connection_state " << state <<endl;
    
}

void messg(const string name,const SocketAddr, const SocketAddr,  shared_ptr<Buffer> buffer, Timestamp time){
    string info(buffer->retrieve_as_String());
    cout <<time.to_formatted_string() << " " << info << endl;
    g_tcpServer->get_tcpConnectionFd(name)->send(time.to_formatted_string() + info);
}



int main(){
    cout << "yixi-test begin" << endl; 
    shared_ptr<EventLoop> loop(EventLoop::construct_eventLoop());
    shared_ptr<TcpServer> tcpServer(TcpServer::construct_TcpServer("ubuntu", "8888", loop.get(), true));
    tcpServer->set_newConnectionCallback(connection_state);
    tcpServer->set_messageCallback(messg);
    tcpServer->start();
    g_tcpServer = tcpServer.get();
    loop->loop();
    cout << "yixi-test end" << endl;
    return 0;
}

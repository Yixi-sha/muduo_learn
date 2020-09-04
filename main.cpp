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
#include "muduo/inc/TcpClient.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;
TcpClient *g_tcpClient;
void my_connect(const string , const SocketAddr local, const SocketAddr peer,TcpConnectionFd::StateE i){
    cout << "connect  " << " " << local.toString() << " " << peer.toString()<<endl;
    g_tcpClient->get_tcpConnectionFd()->send("yixi");
}

void messge(const string ,const SocketAddr, const SocketAddr,  shared_ptr<Buffer> buf, Timestamp){
    cout << "messge " << buf->retrieve_as_String() << endl;
}

int main(){
    cout << "yixi-test begin" << endl; 
    shared_ptr<EventLoop> loop(EventLoop::construct_eventLoop());
    g_loop = loop.get();
    shared_ptr<TcpClient> tcpClient(TcpClient::construct_TcpClient("ubuntu", "8888", g_loop));
    g_tcpClient = tcpClient.get();
    tcpClient->set_connectionCallback(my_connect);
    tcpClient->set_messageCallback(messge);    
    tcpClient->connect();
    loop->loop();
    cout << "yixi-test end" << endl;
    return 0;
}

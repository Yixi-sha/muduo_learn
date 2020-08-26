#ifndef __MY_TCPSERVER_H__
#define __MY_TCPSERVER_H__

#include <string>
#include <memory>
#include <functional>
#include <map>
#include "eventLoop.h"
#include "acceptor.h"
#include "TcpConnection.h"

namespace muduo{

using namespace std;

class TcpConnection;

class TcpServer{
private:
    string name_;
    EventLoop* eventLoop_;
    shared_ptr<Acceptor> acceptor_;
    
    function<void(const shared_ptr<TcpConnection>&)> newConnectionCallback;
    function<void(const shared_ptr<TcpConnection>&, const char*, int)> messageCallback;
    
    map<std::string, shared_ptr<TcpConnection>> connections_;
    int nextConnId_;
    bool started_;
    
public:
    TcpServer();
    ~TcpServer();
};
}




#endif
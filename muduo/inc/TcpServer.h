#ifndef __MY_TCPSERVER_H__
#define __MY_TCPSERVER_H__

#include <string>
#include <memory>
#include <functional>
#include <map>
#include "eventLoop.h"
#include "acceptor.h"
#include "TcpConnectionFd.h"

namespace muduo{

using namespace std;

class TcpServer{
private:
    EventLoop* eventLoop_;
    shared_ptr<Acceptor> acceptor_;
    string name_;
    TcpConnectionFd::ConnectionCallback connectionCallback_;
    TcpConnectionFd::MessageCallback messageCallback_;
    
    map<std::string, shared_ptr<TcpConnectionFd>> connections_;
    int nextConnId_;
    bool started_;
    bool ipv6_;
    TcpServer(string &hostname, string &server, EventLoop* eventLoop, bool ipv6 = false);
    bool construct_two();
    void acceptor_callback(int fd, SocketAddr local, SocketAddr peer);
    void remove_connection(string name);
public:
    static TcpServer* construct_TcpServer(string hostname, string server, EventLoop* eventLoop, bool ipv6 = false);
    ~TcpServer();

    void set_newConnectionCallback(TcpConnectionFd::ConnectionCallback cb);
    void set_messageCallback(TcpConnectionFd::MessageCallback cb);
    bool is_start();
    void start();
    string name() const;
};
}




#endif
#ifndef __MY_TCPCLIENT_H__
#define __MY_TCPCLIENT_H__

#include "noncopyable.h"
#include "TcpConnectionFd.h"
#include "connector.h"

#include <string>

namespace muduo{

using namespace std;

class TcpClient : public Noncopyable{
private:
    EventLoop *eventLoop_;
    shared_ptr<Connector> connector_;
    shared_ptr<TcpConnectionFd> tcpConnectionFd_;

    TcpConnectionFd::ConnectionCallback connectionCallback_;
    TcpConnectionFd::MessageCallback messageCallback_;  
    TcpConnectionFd::WriteCompleteCallback writeCompleteCallback_;
    TcpConnectionFd::HigWaterMarkCallback higWaterMarkCallback_;

    mutable Mutex mutex_;

    bool retry_;
    bool connect_;
    int nextConnId_;
    bool ipv6_;
    TcpClient(string &hostname, string &server, EventLoop *eventLoop, bool ipv6);
    bool construct_two();

    void handle_connect(int socketFd, SocketAddr local, SocketAddr peer);
    void handle_close(string name);
public:
    static TcpClient *construct_TcpClient(string hostname, string server, EventLoop *eventLoop, bool ipv6 = false);
    
    void connect();
    void disconnect();
    void stop();
    
    bool is_retry() const;
    void set_retry(bool on);

    void set_connectionCallback(TcpConnectionFd::ConnectionCallback cb);
    void set_messageCallback(TcpConnectionFd::MessageCallback cb);
    void set_writeCompleteCallback(TcpConnectionFd::WriteCompleteCallback cb);
    void set_higWaterMarkCallback(TcpConnectionFd::HigWaterMarkCallback cb);

    shared_ptr<TcpConnectionFd> get_tcpConnectionFd() const;
    ~TcpClient();
};

}

#endif
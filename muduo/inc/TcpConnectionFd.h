#ifndef __MY_TCPCONNECTIONFD_H__
#define __MY_TCPCONNECTIONFD_H__

#include "object.h"
#include "socketFd.h"
#include "eventLoop.h"
#include "channel.h"
#include "buffer.h"

namespace muduo{
class TcpConnectionFd : public SocketFd{
public:
    enum StateE{
        kConnecting = 0,
        kConnected,
        KDisconnected,
    };  
    using MessageCallback = function<void(const string ,const SocketAddr, const SocketAddr,  Buffer *, Timestamp)>;
    using ConnectionCallback = function<void(const string , const SocketAddr, const SocketAddr, StateE)>;
    using CloseCallback = function<void(string)>;
    
private:
    EventLoop *eventLoop_;
    string name_;
    SocketAddr local_;
    SocketAddr peer_;
    shared_ptr<Channel> channel_;

    StateE state_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

    shared_ptr<Buffer> inputBuf_;

    bool construct_two();

    void set_state(StateE state);
    TcpConnectionFd(int fd, string &name, SocketAddr &local, SocketAddr &peer, EventLoop *eventLoop,bool ipv6);
    void handle_read(Timestamp nowtime);
    void handle_write(Timestamp nowtime);
    void handle_close(Timestamp nowtime);
    void handle_error(Timestamp nowtime);
public:
    static TcpConnectionFd *construct_tcpConnectionFd(int fd, string &name, SocketAddr &local, SocketAddr &peer, EventLoop *eventLoop, bool ipv6 = false);
    ~TcpConnectionFd();

    
    void connection_established();
    void set_newConnectionCallback(ConnectionCallback cb);
    void set_messageCallback(MessageCallback cb);
    void set_closeCallback(CloseCallback cb);
    void connect_destroyed();
};
}

#endif
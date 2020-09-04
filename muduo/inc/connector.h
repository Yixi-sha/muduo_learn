#ifndef __MY_CONNECTOR_H__
#define __MY_CONNECTOR_H__

#include "socketFd.h"
#include "eventLoop.h"
#include <functional>
#include <string>

namespace muduo{

using namespace std;

class Connector : public Noncopyable{
public:
    using NewConnectionCallback = function<void(int, SocketAddr, SocketAddr)>;
    enum States { 
        kDisconnected = 0, 
        kConnecting, 
        kConnected, 
    };
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;
private:
    shared_ptr<SocketAddrInfo> addr_;
    SocketAddr saddr_;

    EventLoop *eventLoop_;
    bool ipv6_;
    States state_;
    bool connect_;
    unique_ptr<Channel> channel_;
    int retryDelayMs_;
    TimerId timerId_;

    NewConnectionCallback newConnectionCallback_;

    Connector(string hostname, string server, EventLoop *eventLoop, bool ipv6);
    bool construct_two();

    void start_in_loop();
    void connect();

    void write_handle();
    void error_handle();
    void connecting(int socketFd);
    int remove_and_reset_channel();
    void reset_channel(Channel *channel);
    void retry(int sockfd);
public:
    static Connector* construct_connector(string hostname, string server, EventLoop *eventLoop, bool ipv6 = false);
    ~Connector();

    void set_newConnectionCallback(NewConnectionCallback cb);
    void start();
    void restart();
    void stop();
    bool channel_is_exist();
};

}

#endif
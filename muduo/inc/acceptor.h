#ifndef __MY_ACCPTOR_H__
#define __MY_ACCPTOR_H__

#include "TcpServerFd.h"
#include "channel.h"
#include "eventLoop.h"

#include <memory>
#include <functional>

namespace muduo{
using namespace std;



class Acceptor : public Noncopyable{
private:
    shared_ptr<TcpServerFd>  tcpServer_;
    function<void(const int, const SocketAddr, const SocketAddr)> acceptCallback_;
    EventLoop *eventLoop_;
    shared_ptr<Channel> channel_;
    bool listening_;

    void handle_read(Timestamp time);
    Acceptor(string &hostnamen, string &server, EventLoop *eventLoop, bool ipv4 = false);
    bool construct_two();
public:
    static Acceptor *construct_accpetor(string hostnamen, string server, EventLoop *eventLoop, bool ipv4 = false);
    ~Acceptor();

    bool is_listening() const{
        return listening_;
    }
    bool listen(int num = 20);
    void set_new_connect_callback(function<void(const int, const SocketAddr, const SocketAddr)> cb );
    SocketAddr get_SocketAddr() const;
};


}


#endif
#ifndef __MY_ACCPTOR_H__
#define __MY_ACCPTOR_H__

#include "TcpServerFd.h"
#include "channel.h"
#include "eventLoop.h"

#include <memory>
#include <functional>

namespace muduo{
using namespace std;

class AcceptFd : public SocketFd{
public:
    AcceptFd(int fd);
};

class Acceptor : public Noncopyable{
private:
    shared_ptr<TcpServerFd>  tcpServer_;
    function<void(AcceptFd)> acceptCallback_;
    EventLoop *eventLoop_;
    shared_ptr<Channel> channel_;
    bool listening_;

    void handle_read();
    Acceptor(string &hostnamen, string &server, EventLoop *eventLoop);
    bool construct_two();
public:
    static Acceptor *construct_accpetor(string hostnamen, string server, EventLoop *eventLoop);
    ~Acceptor();

    bool is_listening() const{
        return listening_;
    }
    bool listen(int num = 20);
    void set_new_connect_callback(function<void(AcceptFd)> cb );
    
};


}


#endif
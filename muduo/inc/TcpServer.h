#ifndef __MY_TCPSERVER_H__
#define __MY_TCPSERVER_H__

#include "noncopyable.h"
#include "socketAddr.h"
#include <memory>

namespace muduo{

using namespace std;

class TcpServer : public Noncopyable{
private:
    unique_ptr<SocketAddr> addr_;
    int fd_;

    TcpServer(string &hostname, string &server, bool ipv6);
    bool construct_two();
public:
    static TcpServer* construct_tcpServer(string hostname, string server, bool ipv6 = false);
    ~TcpServer();
    int get_fd() const;
    bool listen(int num = 20);
    int accpet(sockaddr *addr, socklen_t *slen);
    bool set_reuse_addr(bool on);
    bool set_noblock(bool no);
};
}


#endif
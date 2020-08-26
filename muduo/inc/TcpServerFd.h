#ifndef __MY_TCPSERVERFD_H__
#define __MY_TCPSERVERFD_H__

#include "noncopyable.h"
#include "socketAddr.h"
#include "socketFd.h"

#include <memory>

namespace muduo{

using namespace std;

class TcpServerFd : public SocketFd{
private:
    shared_ptr<SocketAddrInfo> addr_;
    SocketAddr saddr_;
    TcpServerFd(string &hostname, string &server, bool ipv6);
    bool construct_two();
public:
    static TcpServerFd* construct_tcpServerFd(string hostname, string server, bool ipv6 = false);
    ~TcpServerFd();
    
    bool listen(int num = 20);
    int accpet(sockaddr *addr, socklen_t *slen, int flages = 0);

};
}


#endif
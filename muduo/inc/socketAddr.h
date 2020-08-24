#ifndef __MY_SOCKETADDR_H__
#define __MY_SOCKETADDR_H__
#include <string>

#include "object.h"

extern "C"{
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
}


namespace muduo{
using namespace std;

class SocketAddr : public Object{
private:
    string hostname_;
    string server_;
    bool ipv6_;
    int socktype_;
    struct addrinfo *addrinfo_;
    struct addrinfo *addrinfoIt_;
    SocketAddr(string &hostname, string &server, bool ipv6, int socktype);
    bool construct_two();
public:
    static SocketAddr *construct_socketAddr(string &hostname, string &server, bool ipv6 = false,  int socktype = SOCK_STREAM);
    ~SocketAddr();
    void set_it_to_head();
    bool get_addr(struct sockaddr *ai_addr, socklen_t *ai_addrlen) const;
    void next_addr();
    string get_hostname() const;
    string get_server() const;
    int get_now_family() const;
    int get_now_socktype() const;
    int get_now_protocol() const;
};
}


#endif
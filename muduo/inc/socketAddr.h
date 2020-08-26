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

class SocketAddr: public Object {
private:
  struct sockaddr_in addr4_;
  struct sockaddr_in6 addr6_;
  bool ipv6_;
public:

  explicit SocketAddr(uint16_t port, bool ipv6 = false);
  SocketAddr(const string& ip, uint16_t port, bool ipv6 = false);
  SocketAddr(const struct sockaddr_in& addr);
  SocketAddr(const struct sockaddr_in6& addr);
  SocketAddr(const class SocketAddr &addr);

  SocketAddr &operator = (const struct sockaddr_in &addr4);
  SocketAddr &operator = (const struct sockaddr_in6 &addr6);
  SocketAddr &operator = (const class SocketAddr &addr);

  std::string toString() const;
  struct sockaddr_in get_addr4() const;
  struct sockaddr_in6 get_addr6() const;
  bool is_ipv6() const;
};

class SocketAddrInfo : public Object{
private:
    string hostname_;
    string server_;
    bool ipv6_;
    int socktype_;
    struct addrinfo *addrinfo_;
    struct addrinfo *addrinfoIt_;
    SocketAddrInfo(string &hostname, string &server, bool ipv6, int socktype);
    bool construct_two();
public:
    static SocketAddrInfo *construct_SocketAddrInfo(string &hostname, string &server, bool ipv6 = false,  int socktype = SOCK_STREAM);
    ~SocketAddrInfo();
    void set_it_to_head();
    bool it_is_vaild();
    bool get_addr(struct sockaddr *ai_addr, socklen_t *ai_addrlen) const;
    void next_addr();
    string get_hostname() const;
    string get_server() const;
    int get_now_family() const;
    int get_now_socktype() const;
    int get_now_protocol() const;
    socklen_t get_addrlen() const;
    const struct sockaddr *get_addr() const;
};
}


#endif
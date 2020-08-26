#include "../inc/socketAddr.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"

#include <cstring>

namespace muduo{

SocketAddr::SocketAddr(uint16_t port, bool ipv6) : ipv6_(ipv6){
    bzero(&addr4_, sizeof(addr4_));
    bzero(&addr6_, sizeof(addr6_));

    if(ipv6_){
        addr6_.sin6_family = AF_INET6;
        addr6_.sin6_port = htons(port);
        Inet_pton(AF_INET6, "0:0:0:0:0:0:0:0", &addr6_.sin6_addr);
    }else{
        addr4_.sin_family = AF_INET;
        addr4_.sin_port = htons(port);
        Inet_pton(AF_INET, "0.0.0.0", &addr4_.sin_addr);
    }
}
SocketAddr::SocketAddr(const string& ip, uint16_t port, bool ipv6){
    bzero(&addr4_, sizeof(addr4_));
    bzero(&addr6_, sizeof(addr6_));

    if(ipv6_){
        addr6_.sin6_family = AF_INET6;
        addr6_.sin6_port = htons(port);
        Inet_pton(AF_INET6, ip.c_str(), &addr6_.sin6_addr);
    }else{
        addr4_.sin_family = AF_INET;
        addr4_.sin_port = htons(port);
        Inet_pton(AF_INET, ip.c_str(), &addr4_.sin_addr);
    }
}
SocketAddr::SocketAddr(const struct sockaddr_in& addr){
    addr4_ = addr;
    ipv6_ = false;
}

SocketAddr::SocketAddr(const struct sockaddr_in6& addr){
    addr6_ = addr;
    ipv6_ = true;
}

string SocketAddr::toString() const{
    if(ipv6_)
        return string(Sock_ntop(reinterpret_cast<const struct sockaddr*>(&addr6_), sizeof(addr6_)));
    else
        return string(Sock_ntop(reinterpret_cast<const struct sockaddr*>(&addr4_), sizeof(addr4_)));
}

struct sockaddr_in SocketAddr::get_addr4() const{
    return addr4_;
}

struct sockaddr_in6 SocketAddr::get_addr6() const{
    return addr6_;
}

bool SocketAddr::is_ipv6() const{
    return ipv6_;
}

SocketAddr::SocketAddr(const class SocketAddr &addr){
    if(this != &addr){
        addr4_ = addr.addr4_;
        addr6_ = addr.addr6_;
        ipv6_ = addr.ipv6_;
    }
}

SocketAddr &SocketAddr::operator = (const struct sockaddr_in &addr4){
    addr4_ = addr4;
    ipv6_ = false;
    return *this;
}
SocketAddr &SocketAddr::operator = (const struct sockaddr_in6 &addr6){
    addr6_ = addr6;
    ipv6_ = false;
    return *this;
}

SocketAddr &SocketAddr::operator = (const class SocketAddr &addr){
    if(this != &addr){
        addr4_ = addr.addr4_;
        addr6_ = addr.addr6_;
        ipv6_ = addr.ipv6_;
    }
    return *this;
}


SocketAddrInfo::SocketAddrInfo(string &hostname, string &server, bool ipv6, int socktype)
:hostname_(hostname), server_(server), ipv6_(ipv6), addrinfo_(nullptr), socktype_(socktype){
    
}
bool SocketAddrInfo::construct_two(){
    addrinfo_ = Host_serv(hostname_.c_str(), server_.c_str(), ipv6_ ? AF_INET6 : AF_INET, socktype_, AI_PASSIVE);
    if(addrinfo_ == nullptr){
        LOG_ERROR << "Host_serv" << addrinfo_ << endl;
    }
    if(!addrinfo_){
        return false;
    }
    set_it_to_head();
    return true;
}
SocketAddrInfo *SocketAddrInfo::construct_SocketAddrInfo(string &hostname, string &server, bool ipv6, int socktype){
    SocketAddrInfo *ret = new SocketAddrInfo(hostname, server, ipv6, socktype);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        LOG_ERROR << "SocketAddr::construct_socketAddr" << endl;
        return nullptr;
    }
    
    return ret;
}
SocketAddrInfo::~SocketAddrInfo(){
    if(addrinfo_){
        freeaddrinfo(addrinfo_);
    }
}

void SocketAddrInfo::set_it_to_head(){
    addrinfoIt_ = addrinfo_;
}

bool SocketAddrInfo::it_is_vaild(){
    return addrinfoIt_ != nullptr;
}

bool SocketAddrInfo::get_addr(struct sockaddr *ai_addr, socklen_t *ai_addrlen) const{
    if(!addrinfoIt_ || !ai_addr || *ai_addrlen < addrinfoIt_->ai_addrlen){
        LOG_ERROR << "SocketAddr::get_addr" << endl;
        return false;
    }
    memcpy(ai_addr, addrinfoIt_->ai_addr, addrinfoIt_->ai_addrlen);
    *ai_addrlen = addrinfoIt_->ai_addrlen;
    return true;
}

const struct sockaddr *SocketAddrInfo::get_addr() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_addr;
    return nullptr;
}

void SocketAddrInfo::next_addr(){
    if(addrinfoIt_)
        addrinfoIt_ = addrinfoIt_->ai_next;
}

string SocketAddrInfo::get_hostname() const{
    return hostname_;
}
string SocketAddrInfo::get_server() const{
    return server_;
}

int SocketAddrInfo::get_now_family() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_family;
    return -1;
}
int SocketAddrInfo::get_now_socktype() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_socktype;
    return -1;
}
int SocketAddrInfo::get_now_protocol() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_protocol;
    return -1;

}

socklen_t SocketAddrInfo::get_addrlen() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_addrlen;
    return -1;
}



}
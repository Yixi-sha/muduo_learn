#include "../inc/socketAddr.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"

#include <cstring>

namespace muduo{
SocketAddr::SocketAddr(string &hostname, string &server, bool ipv6, int socktype)
:hostname_(hostname), server_(server), ipv6_(ipv6), addrinfo_(nullptr), socktype_(socktype){
    
}
bool SocketAddr::construct_two(){
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
SocketAddr *SocketAddr::construct_socketAddr(string &hostname, string &server, bool ipv6, int socktype){
    SocketAddr *ret = new SocketAddr(hostname, server, ipv6, socktype);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        LOG_ERROR << "SocketAddr::construct_socketAddr" << endl;
        return nullptr;
    }
    
    return ret;
}
SocketAddr::~SocketAddr(){
    if(addrinfo_){
        freeaddrinfo(addrinfo_);
    }
}

void SocketAddr::set_it_to_head(){
    addrinfoIt_ = addrinfo_;
}

bool SocketAddr::get_addr(struct sockaddr *ai_addr, socklen_t *ai_addrlen) const{
    if(!addrinfoIt_ || !ai_addr || *ai_addrlen < addrinfoIt_->ai_addrlen){
        LOG_ERROR << "SocketAddr::get_addr" << endl;
        return false;
    }
    memcpy(ai_addr, addrinfoIt_->ai_addr, addrinfoIt_->ai_addrlen);
    *ai_addrlen = addrinfoIt_->ai_addrlen;
    return true;
}

void SocketAddr::next_addr(){
    if(addrinfoIt_)
        addrinfoIt_ = addrinfoIt_->ai_next;
}

string SocketAddr::get_hostname() const{
    return hostname_;
}
string SocketAddr::get_server() const{
    return server_;
}

int SocketAddr::get_now_family() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_family;
    return -1;
}
int SocketAddr::get_now_socktype() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_socktype;
    return -1;
}
int SocketAddr::get_now_protocol() const{
    if(addrinfoIt_)
        return addrinfoIt_->ai_protocol;
    return -1;

}

}
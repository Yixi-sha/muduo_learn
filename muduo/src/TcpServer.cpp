#include "../inc/TcpServer.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"

extern "C"{
#include <unistd.h>
}

namespace muduo{

TcpServer::TcpServer(string &hostname, string &server, bool ipv6 = false)
:addr_(SocketAddr::construct_socketAddr(hostname, server, ipv6, SOCK_STREAM)),fd_(-1){

}

TcpServer::~TcpServer(){
    ::close(fd_);
}

bool TcpServer::construct_two(){
    sockaddr addr;
    socklen_t slen;
    if(!addr_.get()){
        return false;
    }
    
    for(addr_->set_it_to_head(), slen = sizeof(addr); fd_ == -1 && addr_->get_addr(&addr, &slen);\
    slen = sizeof(addr), addr_->next_addr()){
        fd_ = Socket(addr_->get_now_family(), addr_->get_now_socktype(), addr_->get_now_protocol());
        if(fd_ < 0){
            fd_ = -1;
            continue;
        }
        if(Bind(fd_, &addr, slen)){
            fd_ = -1;
            continue;
        }
    }
    if(fd_ == -1){
        return false;
    }
    
    return true;
}

TcpServer* TcpServer::construct_tcpServer(string hostname, string server, bool ipv6){
    TcpServer* ret = new TcpServer(hostname, server, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        LOG_ERROR << "TcpServer::construct_tcpServer" << endl;
        return nullptr;
    }
    return ret;
}

int TcpServer::get_fd() const{
    return fd_;
}

bool TcpServer::listen(int num){
    if(num <= 0){
        LOG_ERROR << "TcpServer::listen num<0" << endl;
        return false;
    }
    if(Listen(fd_, num) != 0)
        return false;
    return true;
    
}

int TcpServer::accpet(sockaddr *addr, socklen_t *slen){
    return Accept(fd_, addr, slen);
}

bool TcpServer::set_reuse_addr(bool on){
    int opt = on ? 1 : 0;
    if(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        return false;
    return true;
}

bool TcpServer::set_noblock(bool no){
    int nowFlags = fcntl(fd_, F_GETFL, 0);
    if(nowFlags < 0)
        return false;
    if(no){
        nowFlags |= O_NONBLOCK;
    }else{
        nowFlags &= (~O_NONBLOCK);
    }
    if(fcntl(fd_, F_SETFL, nowFlags) < 0)
        return false;
    return true;
}

}
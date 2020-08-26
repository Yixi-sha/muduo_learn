#include "../inc/TcpServerFd.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"


namespace muduo{

TcpServerFd::TcpServerFd(string &hostname, string &server, bool ipv6 = false): saddr_(0, ipv6){
    addr_.reset(SocketAddrInfo::construct_SocketAddrInfo(hostname, server, ipv6, SOCK_STREAM));
    ipv6_ = ipv6;
}

TcpServerFd::~TcpServerFd(){
}

bool TcpServerFd::construct_two(){
    if(!addr_.get()){
        return false;
    }
    
    for(addr_->set_it_to_head(); addr_->it_is_vaild() && fd_ == -1; addr_->next_addr()){
        fd_ = Socket(addr_->get_now_family(), addr_->get_now_socktype(), addr_->get_now_protocol());
        if(fd_ < 0){
            fd_ = -1;
            continue;
        }
        if(Bind(fd_, addr_->get_addr(), addr_->get_addrlen())){
            fd_ = -1;
            continue;
        }
        if(fd_ != -1)
            break;
    }
    if(fd_ == -1){
        return false;
    }
    if(ipv6_){
        sockaddr_in6 in6;
        socklen_t slen = sizeof(in6);
        addr_->get_addr(reinterpret_cast<sockaddr*>(&in6), &slen);
        saddr_ = in6;
    }else{
        cout << "ipv4" << endl;
        sockaddr_in in;
        socklen_t slen = sizeof(in);
        addr_->get_addr(reinterpret_cast<sockaddr*>(&in), &slen);
        saddr_ = in;
    }
    cout << saddr_.toString() << endl;
    //memcpy(&saddr_, addr_->get_addr(), addr_->get_addrlen());
    return true;
}

TcpServerFd* TcpServerFd::construct_tcpServerFd(string hostname, string server, bool ipv6){
    TcpServerFd* ret = new TcpServerFd(hostname, server, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        LOG_ERROR << "TcpServer::construct_tcpServer" << endl;
        return nullptr;
    }
    return ret;
}



bool TcpServerFd::listen(int num){
    if(num <= 0){
        LOG_ERROR << "TcpServer::listen num<0" << endl;
        return false;
    }
    if(Listen(fd_, num) != 0)
        return false;
   
    return true;
    
}

int TcpServerFd::accpet(sockaddr *addr, socklen_t *slen, int flages){
    if(flages == 0)
        return Accept(fd_, addr, slen);
    else
        return Accept4(fd_, addr, slen, flages);
}



}
#include "../inc/acceptor.h"
#include "../inc/poller.h"
#include <cstring>

extern "C"{
#include <unistd.h>
}

namespace muduo{

Acceptor::Acceptor(string &hostnamen, string &server, EventLoop *eventLoop, bool ipv4)
:tcpServer_(TcpServerFd::construct_tcpServerFd(hostnamen, server, ipv4)), eventLoop_(eventLoop),
listening_(false){

}
bool Acceptor::construct_two(){
    if(tcpServer_.get() == nullptr)
        return false;
    channel_ = make_shared<Channel>(eventLoop_, tcpServer_->get_fd());
    
    if(channel_.get() == nullptr)
        return false;
    channel_->set_read_callback(bind(&Acceptor::handle_read, this, placeholders::_1));
    tcpServer_->set_noblock(true);
    tcpServer_->set_cloexec(true);
    return true;
}
Acceptor *Acceptor::construct_accpetor(string hostnamen, string server, EventLoop *eventLoop, bool ipv4){
   
    Acceptor *ret = new Acceptor(hostnamen, server, eventLoop, ipv4);
    
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}
Acceptor::~Acceptor(){

}

bool Acceptor::listen(int num){
    if(!tcpServer_->listen(num)){
        return false;
    }
    cout << "listen" << tcpServer_->get_fd()<< endl;
    channel_->enable_read();
    listening_ = true;
    return true;
}
void Acceptor::set_new_connect_callback(function<void(const int, const SocketAddr, const SocketAddr)> cb){
    acceptCallback_ = cb;
}
void Acceptor::handle_read(Timestamp time){
    sockaddr_in addr4;
    sockaddr_in6 addr6;
    socklen_t slen;
    int fd = -1;
    if(tcpServer_->is_ipv6()){
        slen = sizeof(addr6);
        fd = tcpServer_->accpet(reinterpret_cast<struct sockaddr*>(&addr6), &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    }else{
        slen = sizeof(addr4);
        fd = tcpServer_->accpet(reinterpret_cast<struct sockaddr*>(&addr4), &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    }
    while(fd >= 0){
        cout << fd << endl;
        struct pollfd poll;
        poll.fd = fd;
        poll.events = POLLOUT;
        poll.revents = 0;
        int ret = ::poll(&poll, 1, 0);
        if((poll.revents & POLLOUT) && acceptCallback_){
            LOG_TRACE << "connection arrive in " << time.to_formatted_string() << endl;
            if(tcpServer_->is_ipv6()){
                acceptCallback_(fd, tcpServer_->get_SocketAddr(), addr6);
            }else{
                acceptCallback_(fd, tcpServer_->get_SocketAddr(), addr4);
            }  
        }else{
            LOG_ERROR << "Acceptor::handle_read()" << endl;
            ::close(fd);
            break;
        }
        if(tcpServer_->is_ipv6()){
            slen = sizeof(addr6);
            fd = tcpServer_->accpet(reinterpret_cast<struct sockaddr*>(&addr6), &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        }else{
            slen = sizeof(addr4);
            fd = tcpServer_->accpet(reinterpret_cast<struct sockaddr*>(&addr4), &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        }
    }
}

SocketAddr Acceptor::get_SocketAddr() const{
    return tcpServer_->get_SocketAddr();
}





}

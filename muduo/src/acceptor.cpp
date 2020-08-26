#include "../inc/acceptor.h"
#include "../inc/poller.h"
#include <cstring>

extern "C"{
#include <unistd.h>
}

namespace muduo{

Acceptor::Acceptor(string &hostnamen, string &server, EventLoop *eventLoop)
:tcpServer_(TcpServerFd::construct_tcpServerFd(hostnamen, server)), eventLoop_(eventLoop),
listening_(false){

}
bool Acceptor::construct_two(){
    if(tcpServer_.get() == nullptr)
        return false;
    channel_ = make_shared<Channel>(eventLoop_, tcpServer_->get_fd());
    
    if(channel_.get() == nullptr)
        return false;
    channel_->set_read_callback(bind(&Acceptor::handle_read, this));
    tcpServer_->set_noblock(true);
    tcpServer_->set_cloexec(true);
    return true;
}
Acceptor *Acceptor::construct_accpetor(string hostnamen, string server, EventLoop *eventLoop){
   
    Acceptor *ret = new Acceptor(hostnamen, server, eventLoop);
    
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
void Acceptor::set_new_connect_callback(function<void(AcceptFd)> cb){
    acceptCallback_ = cb;
}
void Acceptor::handle_read(){
    sockaddr addr;
    socklen_t slen;
    int fd = tcpServer_->accpet(&addr, &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    while(fd >= 0){
        cout << fd << endl;
        struct pollfd poll;
        AcceptFd acceptFd(fd);
        poll.fd = fd;
        poll.events = POLLOUT;
        poll.revents = 0;
        int ret = ::poll(&poll, 1, 0);
        if((poll.revents & POLLOUT) && acceptCallback_){
            acceptCallback_(acceptFd);
        }else{
            LOG_ERROR << "Acceptor::handle_read()" << endl;
            ::close(fd);
            break;
        }
        fd = tcpServer_->accpet(&addr, &slen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    }
}


AcceptFd::AcceptFd(int fd){
    fd_ = fd;
       
}


}

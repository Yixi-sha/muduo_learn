#include "../inc/acceptor.h"

namespace muduo{

Acceptor::Acceptor(string &hostnamen, string &server, EventLoop *eventLoop)
:tcpServer_(TcpServer::construct_tcpServer(hostnamen, server)), eventLoop_(eventLoop),
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
    return true;
}
Acceptor *Acceptor::construct_accpetor(string hostnamen, string server, EventLoop *eventLoop){
    Acceptor *ret = new Acceptor(hostnamen, server, eventLoop);
    if(!ret || ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}
Acceptor::~Acceptor(){

}

bool Acceptor::listen(int num){
    if(tcpServer_->listen(num)){
        return false;
    }
    channel_->enable_read();
    listening_ = true;
    return true;
}
void Acceptor::set_new_connect_callbakc(function<void(int, shared_ptr<sockaddr>, socklen_t)> cb){
    acceptCallback_ = cb;
}
void Acceptor::handle_read(){
    shared_ptr<sockaddr> addr = make_shared<sockaddr>();
    socklen_t slen;
    int fd = tcpServer_->accpet(addr.get(), &slen);
    if(acceptCallback_){
        acceptCallback_(fd, addr, slen);
    }
}
}

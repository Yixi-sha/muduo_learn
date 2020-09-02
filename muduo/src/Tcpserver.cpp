#include "../inc/TcpServer.h"

namespace muduo{

extern "C"{
#include <unistd.h>
}

TcpServer::TcpServer(string &hostname, string &server, EventLoop* eventLoop, bool ipv6)
:eventLoop_(eventLoop), acceptor_(Acceptor::construct_accpetor(hostname, server, eventLoop, ipv6)),
nextConnId_(0), ipv6_(ipv6), started_(false), threadsNum_(0), threadPool_(new EventLoopThreadPool(eventLoop, 0)){
    
}

bool TcpServer::construct_two(){
    if(!acceptor_.get()){
        return false;
    }
    name_ =  acceptor_->get_SocketAddr().toString();
    acceptor_->set_new_connect_callback(bind(&TcpServer::acceptor_callback, this, placeholders::_1, placeholders::_2, placeholders::_3));
    return true;
}

TcpServer* TcpServer::construct_TcpServer(string hostname, string server, EventLoop* eventLoop, bool ipv6){
    TcpServer* ret = new TcpServer(hostname, server, eventLoop, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}

TcpServer::~TcpServer(){

}

string TcpServer::name() const{
    return name_;
}

void TcpServer::acceptor_callback(int fd, SocketAddr local, SocketAddr peer){
    LOG_INFO_MY << fd << "\t" << local.toString() << "\t" << peer.toString() << endl;
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "TcpServer::acceptor_callback eventLoop_->is_in_loop_thread()" << endl;
        return;
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", nextConnId_);
    string connName = name_ + buf;
    EventLoop *localLoop = threadPool_->get_next();
    if(localLoop == nullptr){
        err("TcpServer::acceptor_callback");
    }
    shared_ptr<TcpConnectionFd> TcpConnectionFdptr(TcpConnectionFd::construct_tcpConnectionFd(fd, connName, local, peer, localLoop, ipv6_));
    if(TcpConnectionFdptr.get()){
        connections_[connName] = TcpConnectionFdptr;
        TcpConnectionFdptr->set_newConnectionCallback(connectionCallback_);
        TcpConnectionFdptr->set_messageCallback(messageCallback_);
        TcpConnectionFdptr->set_closeCallback(bind(&TcpServer::remove_connection, this, placeholders::_1));
        localLoop->run_in_loop(bind(&TcpConnectionFd::connection_established, TcpConnectionFdptr));
        ++nextConnId_;
    }else{
        LOG_ERROR << "TcpServer::acceptor_callback" <<endl;
        close(fd);
    }
}

void TcpServer::set_newConnectionCallback(TcpConnectionFd::ConnectionCallback cb){
    connectionCallback_ = cb;
}

void TcpServer::set_messageCallback(TcpConnectionFd::MessageCallback cb){
    messageCallback_ = cb;
}

bool TcpServer::is_start(){
    return started_;
}

void TcpServer::start(){
    if(!started_)
        started_ = true;
    if(!acceptor_->is_listening()){
        eventLoop_->run_in_loop(bind(&Acceptor::listen, acceptor_.get(), 20));
    }
    threadPool_->set_thread_num(threadsNum_);
    threadPool_->start();
}

void TcpServer::remove_connection(string name){
    eventLoop_->run_in_loop(bind(&TcpServer::remove_connection_in_loop, this, name));
}

void TcpServer::remove_connection_in_loop(string name){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "TcpServer::remove_connection" << endl;
        return;
    }
    shared_ptr<TcpConnectionFd> tcpConnectionFdPtr = connections_[name];
    connections_.erase(name);
    tcpConnectionFdPtr->get_loop()->queue_in_loop(bind(&TcpConnectionFd::connect_destroyed, tcpConnectionFdPtr));
}

shared_ptr<TcpConnectionFd> TcpServer::get_tcpConnectionFd(const string &name){
    return connections_[name];
}

void TcpServer::set_threadsNum(int num){
    threadsNum_ = num;
}



}
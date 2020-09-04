#include "../inc/TcpClient.h"

extern "C"{
#include <unistd.h>    
}

namespace muduo{
TcpClient::TcpClient(string &hostname, string &server, EventLoop *eventLoop, bool ipv6):
eventLoop_(eventLoop), connector_(Connector::construct_connector(hostname, server, eventLoop, ipv6)),
retry_(false), connect_(false), nextConnId_(0), ipv6_(ipv6){

}

bool TcpClient::construct_two(){
    if(!connector_.get()){
        return false;
    }
    connector_->set_newConnectionCallback(bind(&TcpClient::handle_connect, this, placeholders::_1, placeholders::_2, placeholders::_3));
    return true;
}

TcpClient *TcpClient::construct_TcpClient(string hostname, string server, EventLoop *eventLoop, bool ipv6){
    TcpClient *ret = new TcpClient(hostname, server, eventLoop, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}

TcpClient::~TcpClient(){
    {
        MutexLockGuard guard(mutex_);
        if(tcpConnectionFd_.get()){
            eventLoop_->queue_in_loop(bind(&TcpConnectionFd::connect_destroyed, tcpConnectionFd_));
            tcpConnectionFd_.reset();
        }
    }

    
}

void TcpClient::connect(){
    connector_->start();
    connect_ = true;
}

void TcpClient::disconnect(){
    connect_ = false;
    {
        MutexLockGuard guard(mutex_);
        if(tcpConnectionFd_.get())
            tcpConnectionFd_->set_shutdown();
    }
}

void TcpClient::stop(){
    connector_->stop();
    connect_ = false;
}

void TcpClient::handle_connect(int socketFd, SocketAddr local, SocketAddr peer){
    char buf[4096];
    snprintf(buf, sizeof(buf), ":%s#%d", peer.toString().c_str(), nextConnId_++);
    string name(buf);
    shared_ptr<TcpConnectionFd> tcpConnectionFd(TcpConnectionFd::construct_tcpConnectionFd(socketFd,name,\
    local, peer, eventLoop_, ipv6_));

    if(!tcpConnectionFd.get()){
        LOG_ERROR << "TcpClient::handle_connect" << endl;
        close(socketFd);
        return;
    }
    tcpConnectionFd->set_newConnectionCallback(connectionCallback_);
    tcpConnectionFd->set_messageCallback(messageCallback_);
    tcpConnectionFd->set_writeCompleteCallback(writeCompleteCallback_);
    tcpConnectionFd->set_higWaterMarkCallback(higWaterMarkCallback_);
    tcpConnectionFd->set_closeCallback(bind(&TcpClient::handle_close, this, placeholders::_1));
    {
        MutexLockGuard guard(mutex_);
        tcpConnectionFd_ = tcpConnectionFd;
    }
    tcpConnectionFd_->connection_established();
    
}

void TcpClient::handle_close(string name){
    shared_ptr<TcpConnectionFd> willdelete;
    {
        MutexLockGuard guard(mutex_);
        if(tcpConnectionFd_->get_name() != name || !tcpConnectionFd_.get()){
            LOG_ERROR << "TcpClient::handle_close" << endl;
            return;
        }
        willdelete.swap(tcpConnectionFd_);
    }

    if(willdelete.get()){
        eventLoop_->queue_in_loop(bind(&TcpConnectionFd::connect_destroyed, willdelete));
    }
    if(retry_ && connect_){
        connector_->restart();
    }

}

bool TcpClient::is_retry() const{
    return retry_;
}

void TcpClient::set_retry(bool on){
    retry_ = on;
}

void TcpClient::set_connectionCallback(TcpConnectionFd::ConnectionCallback cb){
    connectionCallback_ = cb;
}

void TcpClient::set_messageCallback(TcpConnectionFd::MessageCallback cb){
    messageCallback_ = cb;
}

void TcpClient::set_writeCompleteCallback(TcpConnectionFd::WriteCompleteCallback cb){
    writeCompleteCallback_ = cb;
}

void TcpClient::set_higWaterMarkCallback(TcpConnectionFd::HigWaterMarkCallback cb){
    higWaterMarkCallback_ = cb;
}

shared_ptr<TcpConnectionFd> TcpClient::get_tcpConnectionFd() const{

    MutexLockGuard guard(mutex_);
    return tcpConnectionFd_;
}

}
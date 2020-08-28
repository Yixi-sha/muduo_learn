#include "../inc/TcpConnectionFd.h"
#include "../inc/wrap_func.h"


namespace muduo{


TcpConnectionFd::TcpConnectionFd(int fd, string &name, SocketAddr &local, SocketAddr &peer, EventLoop *eventLoop, bool ipv6)
:SocketFd(fd, ipv6),eventLoop_(eventLoop), name_(name), local_(local), peer_(peer),
channel_(new Channel(eventLoop_, fd)), state_(kConnecting),inputBuf_(Buffer::construc_buffer()){
    channel_->set_read_callback(bind(&TcpConnectionFd::handle_read, this, placeholders::_1));

}

bool TcpConnectionFd::construct_two(){
    if(!channel_.get() || !inputBuf_.get())
        return false;
    return true;
}

TcpConnectionFd *TcpConnectionFd::construct_tcpConnectionFd(int fd, string &name, SocketAddr &local, SocketAddr &peer, EventLoop *eventLoop, bool ipv6){
    TcpConnectionFd *ret = new TcpConnectionFd(fd, name, local, peer, eventLoop, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}

TcpConnectionFd::~TcpConnectionFd(){

}

void TcpConnectionFd::set_newConnectionCallback(ConnectionCallback cb){
    connectionCallback_ = cb;
}

void TcpConnectionFd::set_messageCallback(MessageCallback cb){
    messageCallback_ = cb;
}

void TcpConnectionFd::set_closeCallback(CloseCallback cb){
    closeCallback_ = cb;
}

void TcpConnectionFd::connection_established(){
    if(!eventLoop_->is_in_loop_thread() || state_ != kConnecting){
        LOG_ERROR << "TcpConnectionFd::connection_established()" << endl;
    }
    set_state(kConnected);
    channel_->enable_read();
    if(connectionCallback_)
        connectionCallback_(name_, local_, peer_, state_);
}

void TcpConnectionFd::set_state(StateE state){
    state_ = state;
}

void TcpConnectionFd::handle_read(Timestamp nowtime){
    char buf[MAXLINE];
    int n = this->read(buf, MAXLINE);
    //cout << nowtime.to_formatted_string() << endl;
    if(n > 0 && messageCallback_){
        //messageCallback_(name_, local_, peer_, buf, n);
    }else if(n == 0){
        handle_close(nowtime);
    }else{
        handle_error(nowtime);
    }
}

void TcpConnectionFd::handle_write(Timestamp nowtime){

}

void TcpConnectionFd::handle_close(Timestamp nowtime){
    if(!eventLoop_->is_in_loop_thread() || state_ != kConnected){
        LOG_ERROR << "TcpConnectionFd::handle_close()" << endl;
        return;
    }
    channel_->disable_all();
    if(closeCallback_)
        closeCallback_(name_);
}

void TcpConnectionFd::handle_error(Timestamp nowtime){
    LOG_ERROR << "TcpConnectionFd::handle_error()" << endl;
}

void TcpConnectionFd::connect_destroyed(){
    if(!eventLoop_->is_in_loop_thread() || state_ != kConnected){
        LOG_ERROR << "TcpConnectionFd::connect_destroyed()" << endl;
        return;
    }
    set_state(KDisconnected);
    channel_->disable_all();
    eventLoop_->remove_channel(channel_.get());
}

}
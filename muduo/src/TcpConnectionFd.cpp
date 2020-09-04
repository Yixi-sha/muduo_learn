#include "../inc/TcpConnectionFd.h"
#include "../inc/wrap_func.h"


namespace muduo{


TcpConnectionFd::TcpConnectionFd(int fd, string &name, SocketAddr &local, SocketAddr &peer, EventLoop *eventLoop, bool ipv6)
:SocketFd(fd, ipv6),eventLoop_(eventLoop), name_(name), local_(local), peer_(peer),
channel_(new Channel(eventLoop_, fd)), state_(kConnecting),inputBuf_(Buffer::construc_buffer()),
outputBuf_(Buffer::construc_buffer()){
    channel_->set_read_callback(bind(&TcpConnectionFd::handle_read, this, placeholders::_1));
    channel_->set_write_callback(bind(&TcpConnectionFd::handle_write, this, placeholders::_1));
}

bool TcpConnectionFd::construct_two(){
    if(!channel_.get() || !inputBuf_.get() || !outputBuf_.get())
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

void TcpConnectionFd::set_writeCompleteCallback(WriteCompleteCallback cb){
    writeCompleteCallback_ = cb;
}

void TcpConnectionFd::set_higWaterMarkCallback(HigWaterMarkCallback cb){
    higWaterMarkCallback_ = cb;
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
    int *err;
    int n = inputBuf_->read_fd(this->get_fd(), err);
    //cout << nowtime.to_formatted_string() << endl;
    if(n > 0 && messageCallback_){
        messageCallback_(name_, local_, peer_, inputBuf_, nowtime);
    }else if(n == 0){
        handle_close(nowtime);
    }else{
        handle_error(nowtime);
    }
}

void TcpConnectionFd::handle_write(Timestamp nowtime){
    if(channel_->is_writing()){
        int n = this->write(outputBuf_->peek(), outputBuf_->readable_bytes());
        if(n > 0){
            outputBuf_->retrieve(n);
            if(outputBuf_->readable_bytes() == 0){
                if(writeCompleteCallback_)
                    writeCompleteCallback_(name_, local_, peer_);
                channel_->disable_write();
                if(state_ == KDisconnecting){
                    shutdown_in_loop();
                }
            }
        }
    }
}

void TcpConnectionFd::handle_close(Timestamp nowtime){
    if(!eventLoop_->is_in_loop_thread() || (state_ != kConnected && state_ != KDisconnecting)){
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
    if(connectionCallback_)
        connectionCallback_(name_, local_, peer_, state_);
    channel_->disable_all();
    eventLoop_->remove_channel(channel_.get());
}

bool TcpConnectionFd::send(const void* buf, int len){
    return send(string(reinterpret_cast<const char*>(buf), len));
}

bool TcpConnectionFd::send(string buf){
    if(state_ == kConnected){
        if(eventLoop_->is_in_loop_thread()){
            send_in_loop(buf);
        }else{
            eventLoop_->run_in_loop(bind(&TcpConnectionFd::send_in_loop, this, buf));
        }
        return true;
    }else{
        return false;
    }
}

void TcpConnectionFd::send_in_loop(string buf){
    int nowWrited = 0;
    if(!channel_->is_writing() && outputBuf_->readable_bytes() == 0){
        nowWrited = this->write(buf.c_str(), buf.size());
        if(nowWrited > 0){
            if(nowWrited < buf.size())
                LOG_TRACE  << "write countinue " << endl;
            else if(nowWrited == buf.size()){
                if(writeCompleteCallback_)
                   writeCompleteCallback_(name_, local_, peer_); 
            }
        }else{
            nowWrited = 0;
            if(errno != EWOULDBLOCK){
                LOG_ERROR << "TcpConnectionFd::send_in_loop" << endl;
            }
        }
    }

    if(nowWrited >= 0 && nowWrited < buf.size()){
        outputBuf_->append(buf.c_str() + nowWrited, buf.size() - nowWrited);
        if(!channel_->is_writing()){
            channel_->enable_write();
        }
        if(outputBuf_->readable_bytes() >= 665535 && higWaterMarkCallback_){
            higWaterMarkCallback_(name_, local_, peer_, outputBuf_->readable_bytes());
        }
    }

}

bool TcpConnectionFd::set_shutdown(){
    if(state_ == kConnected){
        set_state(KDisconnecting);
        eventLoop_->run_in_loop(bind(&TcpConnectionFd::shutdown_in_loop, this));
        return true;
    }else{
        return false;
    }
}

void TcpConnectionFd::shutdown_in_loop(){
    if(!channel_->is_writing()){
        this->shutdown(SHUT_WR);
    }
}

bool TcpConnectionFd::set_tcp_noDelay(bool on){
    int opt = on ? 1 : 0;
    return ::setsockopt(this->get_fd(), IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == 0;
}

bool TcpConnectionFd::set_tcp_keepAlive(bool on, int keppIdle, int keepInterval, int keepCount){
    int opt = on ? 1 : 0;
    bool ret = ::setsockopt(this->get_fd(), SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) == 0;
    if(!on || !ret)
        return ret;

    ret = ::setsockopt(this->get_fd(), SOL_TCP, TCP_KEEPIDLE, &keppIdle, sizeof(keppIdle)) == 0;
    if(!ret)
        return ret;
    ret = ::setsockopt(this->get_fd(), SOL_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval)) == 0;
    if(!ret)
        return ret;
    ret = ::setsockopt(this->get_fd(), SOL_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount)) == 0;

    return ret;
    
}

EventLoop *TcpConnectionFd::get_loop(){
    return eventLoop_;
}

string TcpConnectionFd::get_name() const{
    return name_;
}

}
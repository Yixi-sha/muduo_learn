#include "../inc/connector.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"

namespace muduo{

Connector::Connector(string hostname, string server, EventLoop *eventLoop, bool ipv6):
eventLoop_(eventLoop),
addr_(SocketAddrInfo::construct_SocketAddrInfo(hostname, server, ipv6)), saddr_(0, ipv6),
ipv6_(ipv6), state_(kDisconnected), connect_(false){

}

bool Connector::construct_two(){
    if(!addr_.get())
        return false;
    addr_->set_it_to_head();
    return true;
}

Connector* Connector::construct_connector(string hostname, string server, EventLoop *eventLoop, bool ipv6){
    Connector* ret = new Connector(hostname, server, eventLoop, ipv6);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}

Connector::~Connector(){

}

void Connector::set_newConnectionCallback(NewConnectionCallback cb){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "Connector::set_newConnectionCallback() " << endl;
        return;
    }
    newConnectionCallback_ = cb;
}

void Connector::start_in_loop(){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "Connector::startInLoop() !eventLoop_->is_in_loop_thread()" << endl;
        return;
    }
    if(connect_){
        connect();
    }else{
        LOG_ERROR << "Connector::startInLoop() !connect_" << endl;
    }
}

void Connector::connect(){
    int socketfd = ::socket(addr_->get_now_family(), addr_->get_now_socktype(), addr_->get_now_protocol()); 
    if(socketfd < 0){
        LOG_ERROR << "Connector::connect()" << endl;
        return;
    }
    int ret = ::connect(socketfd, addr_->get_addr(), addr_->get_addrlen());
    int saveErrno = (ret == 0) ? 0 : errno;

    switch(saveErrno){
        case 0 :
        case EINPROGRESS :
        case EINTR :
        case EISCONN :
            connecting(socketfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(socketfd);
            break;
        
        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_ERROR << "Connector::connect() " << saveErrno << endl;
            close(socketfd);
            break;
        default:
            LOG_ERROR << "Connector::connect() default " << saveErrno << endl;
            close(socketfd);
            break;
    }

}

void Connector::connecting(int socketFd){
    state_ = kConnecting;
    if(channel_.get()){
        err("Connector::connectting");
    }
    channel_.reset(new Channel(eventLoop_, socketFd));
    if(!channel_.get()){
        LOG_ERROR << "Connector::connectting" << endl;
        close(socketFd);
        return;
    }
    channel_->set_write_callback(bind(&Connector::write_handle, this));
    channel_->set_error_callback(bind(&Connector::error_handle, this));

    channel_->enable_write();
}

void Connector::start(){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "Connector::start() " << endl;
        return;
    }
    connect_ = true;
    retryDelayMs_ = kInitRetryDelayMs;
    eventLoop_->run_in_loop(bind(&Connector::start_in_loop, this));
}

void Connector::restart(){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "Connector::restart() " << endl;
        return;
    }
    state_ = kDisconnected;
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    eventLoop_->run_in_loop(bind(&Connector::start_in_loop, this));
}

void Connector::stop(){
    connect_ = false;
    if(timerId_.get_value()){
        eventLoop_->cancel_timer(timerId_);
    }
}

int Connector::remove_and_reset_channel(){
    if(!channel_.get()){
        LOG_ERROR << "Connector::remove_and_reset_channel()" << endl;
        return -1;
    }
    channel_->disable_all();
    eventLoop_->remove_channel(channel_.get());
    int ret = channel_->fd();
    Channel *deletePtr = channel_.release();
    eventLoop_->run_in_loop(bind(&Connector::reset_channel, this, deletePtr));
    
    if(channel_.get()){
        LOG_ERROR << "Connector::remove_and_reset_channel()" << endl;
    }

    return ret;
}

void Connector::reset_channel(Channel *channel){
    if(channel)
        delete channel;
}

void Connector::write_handle(){
    if(state_ == kConnecting){
        int socketFd = remove_and_reset_channel();
        int err = Get_socket_error(socketFd);
        if(err){
            LOG_WARN<< "Connector::handleWrite - SO_ERROR = " << err<<endl;
            retry(socketFd);
        }else if(Is_selfConnect(socketFd)){
            LOG_WARN << "Connector::handleWrite - Self connect";
            retry(socketFd);
        }else{
            state_ = kConnected;
            if(connect_ && newConnectionCallback_){
                newConnectionCallback_(socketFd);
            }else{
                close(socketFd);
            }
        }
    }else{
        LOG_ERROR << "Connector::write_handle() " << endl;
    }
}

void Connector::error_handle(){
    LOG_ERROR << "Connector::handleError" << endl;
    int socketFd = remove_and_reset_channel();
    if(state_ == kConnecting && connect_){
        if(socketFd > 0){
            int err = Get_socket_error(socketFd);
            LOG_ERROR << "Connector::error_handle() error: " << err << endl;
            retry(socketFd);
        }
    }else{
        close(socketFd);
    }
}

void Connector::retry(int sockfd){
    close(sockfd);
    state_ = kDisconnected;
    if(connect_){
        LOG_INFO_MY<<" Connector::retry " << addr_->get_hostname() << " " << addr_->get_server() \
        <<" after "<< retryDelayMs_ << endl;

        addr_->next_addr();
        if(!addr_->it_is_vaild()){
            addr_->set_it_to_head();
        }

        timerId_ = eventLoop_->run_after(retryDelayMs_ / 1000.0,bind(&Connector::connect, this));
        retryDelayMs_ = std::min(retryDelayMs_ * 2 , 30*1000);
    }else{
        LOG_DEBUG_MY << "do not connect" << endl;
    }
}

}
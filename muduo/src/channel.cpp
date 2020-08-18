#include "../inc/channel.h"
#include "../inc/log.h"

namespace muduo{

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = POLLIN | POLLPRI;
const int Channel::kWriteEvent_ = POLLOUT;
const int Channel::kErrorEvent_ = POLLERR;

Channel::Channel(EventLoop *eventLoop, int fd)
:eventLoop_(eventLoop), fd_(fd), events_(0), revents_(0),index_(-1){

}

bool Channel::set_read_callback(EventCallback& cb){
    readCallback_ = cb;
    return true;
}

bool Channel::set_write_callback(EventCallback& cb){
    writeCallback_ = cb;
    return true;
}

bool Channel::set_error_callback(EventCallback& cb){
    errorCallback_ = cb;
    return true;
}

int Channel::fd() const{
    return fd_;
}

bool Channel::enable_read(){
    if(!readCallback_){
        return false;
    }
    events_ |= kReadEvent_;
    update();
    return true;
}

void Channel::disable_read(){
    events_ &= ~kReadEvent_;
    update();
}

bool Channel::enable_write(){
    if(!writeCallback_){
        return false;
    }
    events_ |= kWriteEvent_;
    update();
    return true;
}
void Channel::disable_write(){
    events_ &= ~kWriteEvent_;
    update();
}

/*bool Channel::enable_error(){
    if(errorCallback_.target<void()>() == nullptr){
        return false;
    }
    return true;
}*/
//void Channel::disable_error();

bool Channel::enable_all(){
    return enable_read() && enable_write();
}

void Channel::disable_all(){
    disable_read();
    disable_write();
}


int Channel::index() const{
    return index_;
}
bool Channel::set_index(int index){
    index_ = index;
    return true;
}

EventLoop* Channel::owner_loop() const{
    return eventLoop_;
}

void Channel::handle_event(){
    if(revents_ & POLLNVAL){
        LOG_WARN << "channel::handle_event() POLLNVAL";
    }
    if(revents_ & (POLLERR | POLLNVAL)){
        if(errorCallback_)
            errorCallback_();
    }
    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readCallback_)
            readCallback_();
    }
    if(revents_ & POLLOUT){
        if(writeCallback_)
            writeCallback_();
    }
}


}
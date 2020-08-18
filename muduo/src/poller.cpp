#include "../inc/poller.h"

namespace muduo{

Poller::Poller(EventLoop *eventLoop):eventLoop_(eventLoop){

}

Poller::~Poller(){

}

bool Poller::is_in_loop_thread() const{
    return eventLoop_->is_in_loop_thread();
}


}
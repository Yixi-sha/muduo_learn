#include "../inc/eventLoop.h"
#include "../inc/log.h"


using namespace std;
namespace muduo{

__thread EventLoop* loopInThisThread = nullptr;

EventLoop::EventLoop(): tid_(pthread_self()){
    if(loopInThisThread != nullptr){
        LOG_ERROR << "anther eventloop " << loopInThisThread->tid_  <<endl;
    }else{
        loopInThisThread = this;
    }
    looping_ = false;
}

EventLoop::~EventLoop(){
    if(looping_){
       LOG_ERROR << "looping!!!!" << endl;
    }
    loopInThisThread = nullptr;
}

EventLoop* EventLoop::get_event_loop_of_current_thread(){
    return loopInThisThread;
}

void EventLoop::loop(){
    if(looping_)
        LOG_ERROR << "thread looping!" << endl;
    looping_ = true;

    poll(NULL, 0, 5 * 1000);
    cout << "event loop " << this <<" stop looping"<< endl;
    
    looping_ = false;
}

}
#include "../inc/eventLoop.h"


using namespace std;
namespace muduo{

__thread EventLoop* loopInThisThread = nullptr;

EventLoop::EventLoop(): tid_(pthread_self()){
    if(loopInThisThread != nullptr){
       err("anther eventloop " << loopInThisThread->tid_ );
    }else{
        loopInThisThread = this;
    }
    looping_ = false;
}

EventLoop::~EventLoop(){
    if(looping_){
       err("looping!!!!"); 
    }
    loopInThisThread = nullptr;
}

EventLoop* EventLoop::get_event_loop_of_current_thread(){
    return loopInThisThread;
}

void EventLoop::loop(){
    if(looping_)
        err("thread looping!");
    looping_ = true;

    poll(NULL, 0, 5 * 1000);
    cout << "event loop " << this <<" stop looping"<< endl;
    
    looping_ = false;
}

}
#include "../inc/eventLoopThread.h"
#include "../inc/eventLoop.h"

#include <functional>
namespace muduo{
using namespace std;

EventLoopThread::EventLoopThread()
:eventLoop_(NULL), exiting_(false), 
thread_(bind(&EventLoopThread::thread_func, this,  placeholders::_1)),
mutex_(), cond_(mutex_){

}
EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    eventLoop_->quit();
    thread_.join();
}

EventLoop *EventLoopThread::start_loop(){
    if(thread_.is_start()){
        err("EventLoopThread::start_loop() thread_.is_start()");
    }
    thread_.start();
    {
        MutexLockGuard guard(mutex_);
        while(eventLoop_ == nullptr){
            cond_.wait();
        }
    }
    return eventLoop_;
}

void* EventLoopThread::thread_func(void *argv){
    EventLoop eventLoop;
    {
        MutexLockGuard guard(mutex_);
        eventLoop_ = &eventLoop;
        cond_.notify();
    }
    eventLoop.loop();
    return nullptr;
}
}
#include "../inc/eventLoop.h"
#include "../inc/log.h"


using namespace std;
namespace muduo{

__thread EventLoop* loopInThisThread = nullptr;

EventLoop::EventLoop()
: tid_(pthread_self()), looping_(false),quit_(false),poller_(new Poller(this)){
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
    quit_ = false;
    
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(1000, &activeChannels_);
        for(auto &activeChannel : activeChannels_){
            activeChannel->handle_event();
        }
    }

    cout << "event loop " << this <<" stop looping"<< endl;
    
    looping_ = false;
}

void EventLoop::quit(){
    quit_ = true;
}

void EventLoop::update_channel(Channel *channel){
    if(channel->owner_loop() != this){
        
        err("EventLoop::update_channel channel->owner_loop()");
    }
    if(!is_in_loop_thread()){
        err("EventLoop::update_channel is_in_loop_thread()");
    }
    poller_->update_channel(channel);
}

}
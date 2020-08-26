#include "../inc/eventLoop.h"
#include "../inc/log.h"


extern "C"{
#include <sys/timerfd.h>
#include <unistd.h>
#include <sys/eventfd.h>
}


using namespace std;
namespace muduo{

__thread EventLoop* loopInThisThread = nullptr;

int createEventfd()
{
    cout << "s createEventfd"<< endl;
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0){
        LOG_ERROR << "Failed in eventfd" << endl;
        return -1;
    }
    
    cout <<pthread_self() << " evtfd  "<< evtfd << endl;
    return evtfd;
}

EventLoop::EventLoop()
: tid_(pthread_self()), looping_(false),quit_(false),callingPendingFuncBool_(false){
    
}

bool EventLoop::construct_two(){
    if(loopInThisThread != nullptr){
        LOG_ERROR << "anther eventloop " << loopInThisThread->tid_  <<endl;
        return false;
    }else{
        loopInThisThread = this;
    }
    poller_.reset(new Poller(this));
    if(!poller_.get()){
        return false;
    }
    timerQueue_.reset(TimerQueue::construct_timerQueue(this));
    if(!timerQueue_.get()){
        cout << "timerQueue_" << endl;
        return false;
    }
    wakeFd_= createEventfd();
    if(wakeFd_ < 0 )
        return false;
    wakeChannel_.reset(new Channel(this, wakeFd_));
    if(!wakeChannel_.get())
        return false;
     
    
        
   
    wakeChannel_->set_read_callback(bind(&EventLoop::handle_read_wake, this));
    wakeChannel_->enable_read();
    looping_ = false;
    return true;
    
}

EventLoop* EventLoop::construct_eventLoop(){
    EventLoop* ret = new EventLoop();
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;

        return nullptr;
    }
    return ret;
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

bool EventLoop::loop(){
    if(looping_){
        LOG_ERROR << "thread looping!" << endl;
        return false;
    }
    looping_ = true;
    quit_ = false;
    
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(1000, &activeChannels_);
        for(auto &activeChannel : activeChannels_){
            activeChannel->handle_event();
        }
        do_pending_func();
    }
    cout << "event loop " << this <<" stop looping"<< endl;
    looping_ = false;
    return true;
}

void EventLoop::quit(){
    quit_ = true;
    if(!is_in_loop_thread()){
        wakeup();
    }
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

TimerId EventLoop::run_at(const Timestamp &time, const function<void()> &cb){
    return timerQueue_->add_timer(cb, time, 0.0);
}

TimerId EventLoop::run_after(double delay, const function<void()> &cb){
    return run_at(Timestamp::now().add_time(delay), cb);
}

TimerId EventLoop::run_every(double interval, const function<void()> &cb){
    return timerQueue_->add_timer(cb, Timestamp::now().add_time(interval), interval);
}

void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(wakeFd_, &one, sizeof(one));
    if (n != sizeof(one)){
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << endl;
    }
}

void EventLoop::queue_in_loop(const function<void()> &cb){
    {
        MutexLockGuard gurad(pendingFuncMutex_);
        pendingFunc_.push_back(cb);
    }
    if(!is_in_loop_thread() || callingPendingFuncBool_){
        wakeup();
    }
}

void EventLoop::do_pending_func(){
    vector<function<void()>> localPendiengs;
    callingPendingFuncBool_ = true;
    {
        MutexLockGuard gurad(pendingFuncMutex_);
        localPendiengs.swap(pendingFunc_);
    }
    for(auto &localPendieng : localPendiengs){
        localPendieng();
    }
    callingPendingFuncBool_ = false;
}

void EventLoop::run_in_loop(const function<void()> cb){
    if(is_in_loop_thread()){
        cb();
    }else{
        queue_in_loop(cb);
    }
}

void EventLoop::handle_read_wake(){
    uint64_t one = 1;
    ssize_t n = ::read(wakeFd_, &one, sizeof(one));
    if (n != sizeof(one)){
        LOG_ERROR << "EventLoop::handle_read_wake() read " << n << " bytes instead of 8" << endl;
    }
}



}
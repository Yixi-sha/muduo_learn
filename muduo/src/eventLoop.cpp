#include "../inc/eventLoop.h"
#include "../inc/log.h"

extern "C"{
#include <unistd.h>
#include <sys/eventfd.h>
}

using namespace std;
namespace muduo{

__thread EventLoop* loopInThisThread = nullptr;

static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0){
    LOG_ERROR << "Failed in eventfd" << endl;
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop()
: tid_(pthread_self()), looping_(false),quit_(false),poller_(new Poller(this)),
timerQueue_(new TimerQueue(this)), callingPendingFuncBool_(false), wakeFd_(createEventfd()),
wakeChannel_(new Channel(this, wakeFd_)) /*should Second order structure */{
    if(loopInThisThread != nullptr){
        LOG_ERROR << "anther eventloop " << loopInThisThread->tid_  <<endl;
    }else{
        loopInThisThread = this;
    }
    wakeChannel_->set_read_callback(bind(&EventLoop::handle_read_wake, this));
    wakeChannel_->enable_read();
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
        do_pending_func();
    }

    cout << "event loop " << this <<" stop looping"<< endl;
    
    looping_ = false;
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
#include "../inc/timer.h"
#include "../inc/log.h"
#include "../inc/channel.h"
#include "../inc/eventLoop.h"

extern "C"{
#include <unistd.h>
#include <strings.h>
#include <sys/timerfd.h>
}

namespace muduo{
Timer::Timer(const function<void()> &cb, Timestamp when, double interval)
:timeCallback_(cb), expire_(when), interval_(interval), repeat_(interval > 0){

}

void Timer::run() const{
    if(timeCallback_)
        timeCallback_();
}

Timestamp Timer::expire() const{
    return expire_;
}

bool Timer::repeat() const{
    return repeat_;
}

bool Timer::restart(Timestamp now){
    if(repeat_){
        expire_ = now.add_time(interval_);
        return true;
    }else{
        expire_ = Timestamp::invaild();
        return false;
    }
}


/**********************************TimerQueue**************************************/

int createTimerfd(){
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
  if(timerfd < 0){
    LOG_ERROR << "Failed in timerfd_create" << endl;
  }
  return timerfd;
}

void read_timer(int timerfd, Timestamp now){
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    LOG_TRACE << "read_timer " << howmany << "at " << now.to_string() <<endl;
    if(n != sizeof(howmany)){
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8" << endl;
    }
}

struct timespec howMuchTimeFromNow(Timestamp when){
    int64_t microseconds = when.us() - Timestamp::now().us();
    if(microseconds < 100){
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::sToUs);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::sToUs) * 1000);
    return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration){
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret){
        LOG_ERROR << "timerfd_settime()" << endl;
    }
   
}

void TimerQueue::handle_read(){
    Timestamp now(Timestamp::now());
    
    if(!eventLoop_->is_in_loop_thread()){
        err("TimerQueue::handle_read() !eventLoop_->is_in_loop_thread()");
    }
    read_timer(timerFd_, now);
    vector<pair<Timestamp, Timer*>> expireds = get_expired(now);
    for(auto &expired : expireds){
        expired.second->run();
    }
    reset(expireds, now);
}

void TimerQueue::reset(vector<pair<Timestamp, Timer*>> &expireds, Timestamp now){
    for(auto &expired : expireds){
        if(expired.second->restart(now))
            insert(expired.second);
        else
            delete expired.second;
    }
    Timestamp nextExpire = Timestamp::invaild();
    if (!timers_.empty()){
        nextExpire = timers_.begin()->second->expire();
    }
    if (nextExpire.is_valid()){
        resetTimerfd(timerFd_, nextExpire);
    }
}

vector<pair<Timestamp, Timer*>> TimerQueue::get_expired(Timestamp now){
    vector<pair<Timestamp, Timer*>> ret;
    pair<Timestamp, Timer*> sentry = make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    set<pair<Timestamp, Timer*>>::iterator it = timers_.lower_bound(sentry);
    if(it != timers_.end() && now > it->first){
        err("TimerQueue::get_expired it != timers_.end() && now > it->first");
    }
    copy(timers_.begin(), it, back_inserter(ret));
    timers_.erase(timers_.begin(), it);
    return ret;
}

bool TimerQueue::insert(Timer* timer){
    bool earliestChanged = false;
    Timestamp when = timer->expire();
    set<pair<Timestamp, Timer*>>::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first){
        earliestChanged = true;
    }
    pair<set<pair<Timestamp, Timer*>>::iterator, bool> res = timers_.insert(std::make_pair(when, timer));
    if(!res.second){
        err("TimerQueue::insert");
    }
    return earliestChanged;
}

TimerQueue::TimerQueue(EventLoop *eventLoop) 
: eventLoop_(eventLoop), timerFd_(createTimerfd()),channel_(eventLoop_, timerFd_),
timers_(){
    channel_.set_read_callback(bind(&TimerQueue::handle_read, this));
    channel_.enable_read();
}
TimerQueue::~TimerQueue(){
    ::close(timerFd_);
    for(auto &timer : timers_)
        delete timer.second;
}

void TimerQueue::add_timer_in_loop(Timer *timer){
    if(!eventLoop_->is_in_loop_thread()){
        err("TimerQueue::add_timer_in_loop");
    }
    if(insert(timer)){
        resetTimerfd(timerFd_, timer->expire());
    }
}

TimerId TimerQueue::add_timer(const function<void()> cb, Timestamp when, double interval){
    Timer *timer = new Timer{cb ,when, interval};
    if(!timer)
        err("TimerQueue::add_timer");
        
    eventLoop_->run_in_loop(bind(&TimerQueue::add_timer_in_loop, this, timer));
    return TimerId(timer);
}

// void cancel(TimerId timerId);
}
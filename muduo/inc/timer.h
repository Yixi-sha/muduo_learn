#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

#include "timestamp.h"
#include "noncopyable.h"
#include "channel.h"

#include <functional>
#include <set>
#include <vector>

namespace muduo{
using namespace std;
class Timer : Noncopyable{
private:
function<void()> timeCallback_;
Timestamp expire_;
const double interval_;
const bool repeat_;
public:
    Timer(const function<void()> &cb, Timestamp when, double interval);
    ~Timer(){

    }

    void run() const;
    Timestamp expire() const;
    bool repeat() const;
    bool restart(Timestamp now);

};

class TimerId{
private:
    Timer* value_;
public:
    explicit TimerId(Timer* timer): value_(timer){
    }
};

class EventLoop;


class TimerQueue : Noncopyable{
private:
    EventLoop *eventLoop_;
    const int timerFd_;
    Channel channel_;
    set<pair<Timestamp, Timer*>> timers_;

    void handle_read();
    vector<pair<Timestamp, Timer*>> get_expired(Timestamp now);
    bool insert(Timer* timer);
    void reset(vector<pair<Timestamp, Timer*>> &expireds, Timestamp now);
    void add_timer_in_loop(Timer *timer);
public:
    TimerQueue(EventLoop *eventLoop);
    ~TimerQueue();

    TimerId add_timer(const function<void()> cb, Timestamp when, double interval);
    //void cancel(TimerId timerId);
};
}

#endif
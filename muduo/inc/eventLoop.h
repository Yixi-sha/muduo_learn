#ifndef __MY_EVENTLOOP_H__
#define __MY_EVENTLOOP_H__

#include "noncopyable.h"
#include <iostream>
#include <memory>
#include <vector>
#include "log.h"

extern "C"{
#include <pthread.h>
#include <poll.h>
}

#include "poller.h"
#include "channel.h"
#include "timer.h"
#include "mutex.h"

namespace muduo{

class EventLoop : Noncopyable{
private:
    const pthread_t tid_;
    bool looping_;
    bool quit_;
    unique_ptr<Poller> poller_;
    vector<Channel*> activeChannels_;
    unique_ptr<TimerQueue> timerQueue_;

    bool callingPendingFuncBool_;
    int wakeFd_;
    unique_ptr<Channel> wakeChannel_;
    Mutex pendingFuncMutex_;
    vector<function<void()>> pendingFunc_;

    void queue_in_loop(const function<void()> &cb);
    void wakeup();
    void do_pending_func();
    void handle_read_wake();
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void update_channel(Channel *channel);

    bool is_in_loop_thread() const{
        return tid_ == pthread_self();
    }

    TimerId run_at(const Timestamp &time, const function<void()> &cb);
    TimerId run_after(double delay, const function<void()> &cb);
    TimerId run_every(double interval, const function<void()> &cb);

    static EventLoop* get_event_loop_of_current_thread();

    void run_in_loop(const function<void()> cb);
};

}



#endif
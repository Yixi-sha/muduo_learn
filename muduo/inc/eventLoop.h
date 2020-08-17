#ifndef __MY_EVENTLOOP_H__
#define __MY_EVENTLOOP_H__

#include "noncopyable.h"
#include <iostream>

extern "C"{
#include <pthread.h>
#include <poll.h>
}

namespace muduo{
class EventLoop : Noncopyable{
private:
    const pthread_t tid_;
    bool looping_;
public:
    EventLoop();
    ~EventLoop();

    void loop();

    bool is_in_loop_thread() const{
        return tid_ == pthread_self();
    }

    static EventLoop* get_event_loop_of_current_thread();
};

}



#endif
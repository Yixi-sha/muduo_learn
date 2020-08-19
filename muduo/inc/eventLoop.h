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

namespace muduo{

class EventLoop : Noncopyable{
private:
    const pthread_t tid_;
    bool looping_;
    bool quit_;
    unique_ptr<Poller> poller_;
    vector<Channel*> activeChannels_;
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void update_channel(Channel *channel);

    bool is_in_loop_thread() const{
        return tid_ == pthread_self();
    }

    static EventLoop* get_event_loop_of_current_thread();
};

}



#endif
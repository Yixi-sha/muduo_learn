#ifndef __MY_EVENTLOOPTHREAD_H__
#define __MY_EVENTLOOPTHREAD_H__

#include "noncopyable.h"
#include "thread.h"
#include "mutex.h"
#include "condition.h"

namespace muduo{
class EventLoop;

class EventLoopThread : Noncopyable{
private:
    EventLoop *eventLoop_;
    bool exiting_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;

    void* thread_func(void *argv);
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop *start_loop();
};

}

#endif
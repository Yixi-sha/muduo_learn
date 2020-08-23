#ifndef __MY_EVENTLOOPTHREAD_H__
#define __MY_EVENTLOOPTHREAD_H__

#include "noncopyable.h"
#include "thread.h"
#include "mutex.h"
#include "condition.h"

#include <memory>

namespace muduo{
using namespace std;
class EventLoop;

class EventLoopThread : public Noncopyable{
private:
    shared_ptr<EventLoop> eventLoop_;
    bool exiting_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;

    void* thread_func(void *argv);
public:
    EventLoopThread();
    ~EventLoopThread();
    shared_ptr<EventLoop> start_loop();
};

}

#endif
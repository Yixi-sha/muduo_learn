#ifndef __MY_EVENTLOOPTHREADPOOL_H__
#define __MY_EVENTLOOPTHREADPOOL_H__

#include "noncopyable.h"
#include "eventLoopThread.h"
#include <vector>
#include <memory>

namespace muduo{

using namespace std;

class EventLoopThreadPool : public Noncopyable{
private:
    EventLoop *baseloop_;
    bool started_;
    int threadsNum_;
    int next_;

    vector<shared_ptr<EventLoopThread>> threads;
    vector<EventLoop*> loops_;
public:
    EventLoopThreadPool(EventLoop *baseloop, int num = 5);
    ~EventLoopThreadPool();

    int start();
    void set_thread_num(int num);
    EventLoop* get_next();
};

}



#endif
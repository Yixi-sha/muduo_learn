#include "../inc/eventLoopThreadPool.h"
#include "../inc/eventLoop.h"

namespace muduo{

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, int num)
:baseloop_(baseloop), threadsNum_(num), next_(0), started_(false){

}
EventLoopThreadPool::~EventLoopThreadPool(){

}

int EventLoopThreadPool::start(){
    int nowThreadsNum = loops_.size();
    started_ = true;
    for(int i = nowThreadsNum; i < threadsNum_; ++i){
        shared_ptr<EventLoopThread>  eventLoopThread(new EventLoopThread());
        if(!eventLoopThread.get()){
            threadsNum_ = i;
            return threadsNum_;
        }
        EventLoop *loop = eventLoopThread->start_loop().get();
        if(!loop){
            threadsNum_ = i;
            return threadsNum_;
        }
        threads.push_back(eventLoopThread);
        loops_.push_back(loop);
    }
    return threadsNum_;
    
}

void EventLoopThreadPool::set_thread_num(int num){
    threadsNum_ = num;
}

EventLoop* EventLoopThreadPool::get_next(){
    if(!baseloop_->is_in_loop_thread())
        return nullptr;
    if(!started_ || loops_.size() == 0)
        return baseloop_;
    if(next_ == loops_.size())
        next_ = 0;
    return loops_[next_++];
}

}
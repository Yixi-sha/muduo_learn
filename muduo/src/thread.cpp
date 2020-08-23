#include "../inc/thread.h"
#include "../inc/log.h"

namespace muduo{
class ThreadTwo{
private:
    function<void*(void*)> func_;
    void *argv_;
public:
    ThreadTwo(function<void*(void*)> &func, void *argv)
    :func_(func), argv_(argv){

    }
    void* run(){
        return func_(argv_);
    }
};   

static void *thread_func(void *argv){
    ThreadTwo* threadTwo= reinterpret_cast<ThreadTwo*>(argv);
    void * ret = threadTwo->run();
    delete threadTwo;
    return ret;
}

int Thread::start(){
    int ret;
    argv_ = new ThreadTwo(func_, argv_);
    if(!argv_){
        LOG_ERROR <<  "Thread::start()" << endl;
        return -1;
    }
    start_ = true;
    if((ret = pthread_create(&tid_, NULL, thread_func, argv_)) != 0){
        delete reinterpret_cast<ThreadTwo*>(argv_);
    }
    return ret;
}

}
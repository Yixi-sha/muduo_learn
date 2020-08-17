#ifndef __MY_THREAD_H__
#define __MY_THREAD_H__

#include "noncopyable.h"

extern "C"{
#include <pthread.h>
}

namespace muduo{
class Thread : Noncopyable{
private:
    void* (*func_)(void*);
    pthread_t tid_;
    void *argc_;
public:
    Thread(void* (*func)(void*), void *argc = nullptr)
    : func_(func), tid_(0), argc_(argc){

    }
    int start(){
        return pthread_create(&tid_, NULL, func_, argc_);
    }
    pthread_t tid(){
        return tid_;
    }
    int join(void *status){
        if(status == NULL){
            return pthread_join(tid_, nullptr);
        }else{
            return pthread_join(tid_, &status);
        }
    }
};
}


#endif
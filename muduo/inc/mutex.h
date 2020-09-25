#ifndef __MY_MUTEX_H__
#define __MY_MUTEX_H__

#include <iostream>

#include "noncopyable.h"

extern "C"{
#include <pthread.h>
#include <stdlib.h>
}

#include <atomic>

using namespace std;

namespace muduo{
class Mutex : public Noncopyable{
private:
    pthread_mutex_t mutex_;
    pthread_t holder_;
    atomic_int  locked_;
public:
    Mutex() : mutex_(PTHREAD_MUTEX_INITIALIZER), holder_(0), locked_(0){

    }
    int lock(){
        if(locked_ == 1 && holder_ == pthread_self())
            abort();
        int ret = pthread_mutex_lock(&mutex_);
        if(ret == 0){
            locked_ = 1;
            holder_ = pthread_self();
        }
        return ret;
    }
    int unlock(){
        holder_ = 0;
        locked_ = 0;
        return pthread_mutex_unlock(&mutex_);
    }
    bool is_hold_by_self(){
        return holder_ == pthread_self();
    }
    pthread_mutex_t *get_mutex(){
        return &mutex_;
    }
    bool if_locked(){
        return locked_;
    }
};

class MutexLockGuard : Noncopyable{
private:
    Mutex &mutex_;
public:
    MutexLockGuard(Mutex &mutex): mutex_(mutex){
        mutex_.lock();
    }
    ~MutexLockGuard(){
        mutex_.unlock();
    }
};

}


#endif
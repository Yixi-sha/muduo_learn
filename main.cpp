#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>


#include "muduo/inc/mutex.h"
#include "muduo/inc/signal_slot.h"
#include "muduo/inc/blockingQueue.h"
#include "muduo/inc/thread.h"

extern "C"{
#include <unistd.h>
}

using namespace std;
using namespace muduo;

BlockingQueue<int> blockingQueue;

void *thread_func_sum(void *arg){
    //cout << pthread_self() << endl;
    while(1){
        blockingQueue.dequeue();
        cout << "get " << arg <<endl;
    }
    return NULL;
}

void *thread_func_product(void *arg){
    //cout << pthread_self() << endl;
    while(1){
        blockingQueue.enqueue(1);
        cout << "set " << arg<<endl;
        //sleep(1);
    }
    return NULL;
}


int main(){
    Thread s1(thread_func_sum, (void*)1);
    Thread s2(thread_func_sum, (void*)2);
    Thread p1(thread_func_product, (void*)1);
    Thread p2(thread_func_product, (void*)2);
    Thread p3(thread_func_product, (void*)3);
    Thread p4(thread_func_product, (void*)4);
    s1.start();
    s2.start();
    p1.start();
    p2.start();
    p3.start();
    p4.start();
    pause();
    return 0;
}

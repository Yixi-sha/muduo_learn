#include <iostream>

#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"

using namespace std;
using namespace muduo;

void *func(void *){
    Mutex mutex;
    cout << "this is func " << pthread_self() << endl;
    MutexLockGuard gurad(mutex);
    cout << "lock " << pthread_self() << endl;
    cout << "end " << pthread_self() << endl;
    return nullptr;
}

int main()
{

    cout << "yixi-test begin" << endl;
    Thread thread(func);
    thread.start();
    cout << "this is main " << pthread_self() << endl;
    thread.join(nullptr);
    cout << "yixi-test end" << endl;

    return 0;
}

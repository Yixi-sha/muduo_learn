#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/eventLoop.h"

using namespace std;
using namespace muduo;


void *func(void*){
    EventLoop loop;
    EventLoop loop1;
    loop.loop();
    return NULL;
}

int main()
{

    cout << "yixi-test begin" << endl;
    Thread thread(func);
    EventLoop loop;
    thread.start();
    loop.loop();
    pthread_exit(0);

    cout << "yixi-test end" << endl;

    return 0;
}

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/eventLoop.h"
#include "muduo/inc/timestamp.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
}


void *func(void*){
    Timestamp ts = Timestamp::now();

    Timestamp ts1 = ts.add_time(10);
    cout << ts.to_formatted_string() << endl;
    cout << ts1.to_formatted_string() << endl;
    cout << (ts1 == ts) << endl;
    cout << ts1.diff(ts) << endl;
    return NULL;
}

int main()
{

    cout << "yixi-test begin" << endl;
    Thread thread(func);
    thread.start();
    pthread_exit(0);

    cout << "yixi-test end" << endl;

    return 0;
}

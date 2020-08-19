#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

extern "C"{
#include <sys/timerfd.h>
}


#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/eventLoop.h"
#include "muduo/inc/timestamp.h"
#include "muduo/inc/poller.h"
#include "muduo/inc/channel.h"

using namespace std;
using namespace muduo;

extern "C"{
#include <unistd.h>
#include <strings.h>
}

EventLoop *g_loop;

void timeout(){
    cout << "timeout" << endl;
    g_loop->quit();
}

int main()
{

    cout << "yixi-test begin" << endl;
    EventLoop localLoop;
    g_loop = &localLoop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&localLoop, timerfd);
    channel.set_read_callback(timeout);
    channel.enable_read();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(struct itimerspec));
    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd, 0, &howlong, NULL);

    localLoop.loop();
    cout << "yixi-test end" << endl;

    return 0;
}

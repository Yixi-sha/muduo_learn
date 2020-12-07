#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>


#include "muduo/inc/mutex.h"
#include "muduo/inc/signal_slot.h"
#include "muduo/inc/blockingQueue.h"
#include "muduo/inc/thread.h"
#include "muduo/inc/currentThread.h"
#include "muduo/inc/fixedBuffer.h"
#include "muduo/inc/log.h"

extern "C"{
#include <unistd.h>
}

using namespace std;
using namespace muduo;

int main(){
    FixedBuffer<kSmallBuffer> buf;
    LOG_FATAL << "yixi " << 131 << " bingge" << muduo::endl;

    return 0;
}

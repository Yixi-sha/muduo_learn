#ifndef __MY_CHANNLE_H__
#define __MY_CHANNLE_H__

#include <functional>

#include "eventLoop.h"
#include "noncopyable.h"

namespace muduo{
class Channel : Noncopyable{
public:
    typedef std::function<void()> EventCallback;
private:
    const int fd_;
    EventLoop *eventLoop_;

    int events_;
    int revents_;
    int index_;
    
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;

    static const int kNoneEvent_;
    static const int kReadEvent_;
    static const int kWriteEvent_;
    static const int kErrorEvent_;
public:
    Channel();
    ~Channel();
    
};

}



#endif
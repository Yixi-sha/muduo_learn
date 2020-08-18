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

    void update();
public:
    Channel(EventLoop *eventLoop, int fd);
    ~Channel();

    void handle_event();
    bool set_read_callback(EventCallback& cb);
    bool set_write_callback(EventCallback& cb);
    bool set_error_callback(EventCallback& cb);

    int fd() const;

    int events() const;
    bool set_revents(int revent);
    bool is_none_event() const;
    
    bool enable_read();
    void disable_read();
    bool enable_write();
    void disable_write();
    //bool enable_error();
    //void disable_error();
    bool enable_all();
    void disable_all();
    
    int index() const;
    bool set_index(int index);

    EventLoop* owner_loop() const;
};

}



#endif
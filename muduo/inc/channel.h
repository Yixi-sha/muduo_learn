#ifndef __MY_CHANNLE_H__
#define __MY_CHANNLE_H__

#include <functional>


#include "noncopyable.h"
#include "timestamp.h"

namespace muduo{
class EventLoop;

class Channel : public Noncopyable{
public:
    typedef std::function<void(Timestamp)> EventCallback;
private:
    const int fd_;
    EventLoop *eventLoop_;

    int events_;
    int revents_;
    int index_;
    
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    static const int kNoneEvent_;
    static const int kReadEvent_;
    static const int kWriteEvent_;
    static const int kErrorEvent_;

    bool handling_;

    void update();
public:
    Channel(EventLoop *eventLoop, int fd);
    ~Channel();

    void handle_event(Timestamp time);
    bool set_read_callback(const EventCallback& cb);
    bool set_write_callback(const EventCallback& cb);
    bool set_error_callback(const EventCallback& cb);

    int fd() const;

    int events() const;
    bool set_revents(int revent);
    bool is_none_event() const;
    
    bool enable_read();
    void disable_read();
    bool enable_write();
    void disable_write();
    bool enable_all();
    void disable_all();
    bool is_writing() const;
    
    int index() const;
    bool set_index(int index);

    EventLoop* owner_loop() const;
};

}



#endif
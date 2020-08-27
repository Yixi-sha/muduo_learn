#ifndef __MY_POLLER_H__
#define __MY_POLLER_H__

#include "channel.h"
#include "timestamp.h"

#include <vector>
#include <map>

extern "C"{
#include <poll.h>
}

namespace muduo {

using namespace std;

class Poller : public Noncopyable{
private:
    EventLoop *eventLoop_;

    vector<struct pollfd> pollfds_;
    map<int, Channel*> channels_;

    void fill_active_channels(int eventNum, vector<Channel*> *activeChannels);
public:
    Poller(EventLoop *eventLoop);
    ~Poller();

    Timestamp poll(int timeOutMs, vector<Channel*> *activeChannels);
    bool update_channel(Channel *channel);
    bool is_in_loop_thread() const;
    bool remove_channel(Channel* channel);
};

}

#endif
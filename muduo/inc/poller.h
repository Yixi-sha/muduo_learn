#ifndef __MY_POLLER_H__
#define __MY_POLLER_H__

#include "channel.h"

#include <vector>
#include <map>

namespace muduo {

using namespace std;

class Poller : Noncopyable{
private:
    EventLoop *eventLoop_;

    vector<struct pollfd> pollFdList_;
    map<int, Channel*> channels_;

    void fill_active_channels(int eventNum, vector<Channel*> *activeChannels) const;
public:
    Poller(EventLoop *eventLoop);
    ~Poller();

    //Timestamp poll(int timeOutMs, vector<Channel*> *activeChannels);
    void update_channel(Channel *channel);
    bool is_in_loop_thread() const;
};

}

#endif
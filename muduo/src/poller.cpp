#include "../inc/poller.h"

#include "eventLoop.h"
#include "channel.h"

namespace muduo{

Poller::Poller(EventLoop *eventLoop):eventLoop_(eventLoop){

}

Poller::~Poller(){

}

bool Poller::is_in_loop_thread() const{
    return eventLoop_->is_in_loop_thread();
}

Timestamp Poller::poll(int timeOutMs, vector<Channel*> *activeChannels){
    int eventsNum = ::poll(&*pollfds_.begin(), pollfds_.size(), timeOutMs);
    Timestamp now(Timestamp::now());
    if(eventsNum > 0){
        LOG_TRACE << eventsNum << " events happend" << endl;
        fill_active_channels(eventsNum, activeChannels);
    }else if(eventsNum == 0){
        LOG_TRACE << " nothing happend" << endl;
    }else {
        LOG_ERROR << "Poller::poll" << endl;
    }
    return now;
}

void Poller::fill_active_channels(int eventNum, vector<Channel*> *activeChannels){
    for(vector<struct pollfd>::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && eventNum > 0 ; ++pfd){
        if(pfd->revents > 0){
            --eventNum;
            map<int ,Channel*>::const_iterator ch = channels_.find(pfd->fd);
            Channel *channel = ch->second;
            if(pfd->fd != channel->fd()){
                err("pfd->fd != channel->fd()");
            }
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

bool Poller::update_channel(Channel* channel){
    if(!channel){
        return false;
    }
    //cout << "update" << endl;
    if(!is_in_loop_thread()){
        LOG_ERROR << "Poller::update_channel is_in_loop_thread()" << endl;
        return false;
    }
        
    if(channel->index() == -1){
        if(channels_.find(channel->fd()) != channels_.end()){
            LOG_ERROR << "Poller::update_channel  channels_.find(channel->fd) != channels_.end()" << endl;
            LOG_ERROR << channel->index() << endl;
            return false;
        }
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        channel->set_index(static_cast<int>(pollfds_.size() - 1));
        channels_[pfd.fd] = channel;
    }else{
        int idx = channel->index();
        if(channels_.find(channel->fd()) == channels_.end()){
            LOG_ERROR <<  "Poller::update_channel  channels_.find(channel->fd) == channels_.end()" << endl;
            return false;
        }
        if(idx < 0 || idx >= pollfds_.size()){
            LOG_ERROR << "Poller::update_channel  idx < 0 || idx >= pollfds_.size()" << endl;
            return false;
        }
            
        if(channels_[channel->fd()] != channel){
            LOG_ERROR <<"Poller::update_channel  channels_[channel->fd()] != channel" << endl;
            return false;
        }
        struct pollfd &pfd = pollfds_[idx];
        if(pfd.fd != -1 || pfd.fd != channel->fd()){
            LOG_ERROR << "Poller::update_channel pfd.fd != -1 || pfd.fd != channel->fd()" << endl;
            return false;
        }
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pfd.fd = channel->fd();
        if(channel->is_none_event()){
            pfd.fd = -1;
        }
    }
    return true;
}

}
#ifndef __MY_SIGNAL_SOLT_H__
#define __MY_SIGNAL_SOLT_H__

#include <functional>
#include <vector>
#include <memory>

#include "noncopyable.h"
#include "mutex.h"

namespace muduo{

using namespace std;

template<typename Callback>
class Signal;

template<typename Callback>
class Slot : public Noncopyable{
public:
    using Data = Signal<Callback>;
private:
    weak_ptr<Data> wpData_;
    
    Callback cb_;
    bool tied_;
    weak_ptr<void> wpTie_;
public:
    Slot(const weak_ptr<Data> &data, Callback &&cb):
    wpData_(data), cb_(cb), wpTie_(), tied_(false){

    }

    Slot(const weak_ptr<Data> &data, Callback &&cb, weak_ptr<void> &tie):
    wpData_(data), cb_(cb), wpTie_(tie), tied_(true){

    }
    bool is_tied() const{
        return tied_;
    }

    weak_ptr<void>  wp_tie() const{
        return wpTie_;
    }

    Callback callback() const{
        return cb_;
    }
    
    weak_ptr<Data> wp_data() const{
        return wpData_;
    }

    ~Slot();
};

template<typename Callback>
class Signal : public Noncopyable{
private:
    using Slots = vector<weak_ptr<Slot<Callback>>>;

    Mutex mutex_;
    shared_ptr<Slots> spSlots_;

    Signal():
    spSlots_(new Slots){

    }
    bool construct_two(){
        if(!spSlots_.get())
            return false;
        return true;
    }

    bool copy_on_write(){
        if(!spSlots_.unique()){
            shared_ptr<Slots> spSlots(new Slots(*spSlots_));
            if(!spSlots.get())
                return false;
            spSlots_ = spSlots;
        }
        return true;
    }
public:
    static Signal* construct_signal(){
        Signal* ret = new Signal();
        if(!ret || !ret->construct_two()){
            if(ret)
                delete ret;
            return nullptr;
        }
        return ret;
    }

    bool clean(){
        MutexLockGuard guard(mutex_);
        if(!copy_on_write())
            return false;
        typename Slots::iterator it = spSlots_->begin();
        while(it != spSlots_->end()){
            if(it->expired())
                spSlots_->erase(it);
            else
                ++it;
        }
        return true;
    }

    bool add(shared_ptr<Slot<Callback>> slot){
        MutexLockGuard guard(mutex_);
        if(!copy_on_write())
            return false;
        spSlots_->push_back(slot);
        return true;
    }

    template<typename... ARGS>
    void call(ARGS&&... args){
        shared_ptr<Slots> spSlots;
        {
            MutexLockGuard guard(mutex_);
            spSlots = spSlots_;
        }
        
        typename Slots::iterator it = spSlots->begin(); 
        
        while(it != spSlots->end()){
            shared_ptr<Slot<Callback>> slot = it->lock();
            if(slot.get()){
                shared_ptr<void> guard;
                if(slot->is_tied()){
                    guard = slot->wp_tie().lock();
                    if(guard.get()){
                        slot->callback()(args...);
                    }
                }else{
                    slot->callback()(args...);
                }
            }
            ++it;
        }
    }
};

template<typename Callback>
Slot<Callback>::~Slot(){
    shared_ptr<Data> spData(wpData_);
    if(spData.get()){
        spData->clean();
    }
}

template< typename... ARGS>
class SingalUsed: public Noncopyable{
public:
    using Callback = function<void(ARGS...)>;
    using Signal_ = Signal<Callback>;
    using Slot_ = Slot<Callback>;
    using SlotRet = shared_ptr<void>;
private:
    shared_ptr<Signal_> spSignal_;

    SingalUsed() : spSignal_(Signal_::construct_signal()){

    }
    bool construct_two(){
        if(!spSignal_.get())
            return false;
        return true;
    } 
public:
    static SingalUsed* construct_signalUsed(){
        SingalUsed* ret = new SingalUsed();
        if(!ret || !ret->construct_two()){
            if(ret)
                delete ret;
            return nullptr;
        }
        return ret;
    }

    ~SingalUsed(){

    }

    SlotRet connect(Callback &&cb){
        shared_ptr<Slot_> slot(new Slot_(spSignal_, std::forward<Callback>(cb)));
        if(!slot.get()){
            return slot;
        }
        if(!spSignal_->add(slot)){
            slot.reset();
        }
        return slot;
    }

    SlotRet connect(Callback &&cb, const shared_ptr<void>& tie){
        shared_ptr<Slot_> slot(new Slot_(spSignal_, std::forward<Callback>(cb)), tie);
        if(!slot.get()){
            return slot;
        }
        if(!spSignal_->add(slot)){
            slot.reset();
        }
        return slot;
    }

    void call(ARGS&&... args){
        spSignal_->call(args...);
    }
   
};

}


#endif
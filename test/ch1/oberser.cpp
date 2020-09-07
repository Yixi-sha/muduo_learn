#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>


#include "muduo/inc/mutex.h"

using namespace std;
using namespace muduo;

class Observer{
public:
    ~Observer(){

    }

    void update(){
        cout << "Observer::update()" << endl;
    }
};

class Obserable{
    Mutex mutex_;
    vector<weak_ptr<Observer>> observers_;
public:
    void register_(weak_ptr<Observer> x){
        MutexLockGuard guard(mutex_);
        observers_.push_back(x);
    }

    void notify(){
        MutexLockGuard guard(mutex_);
        vector<weak_ptr<Observer>>::iterator it = observers_.begin();
        while(it != observers_.end()){
            shared_ptr<Observer> obj(it->lock());
            if(obj.get()){
                obj->update();
                ++it;
            }else{
                observers_.erase(it);
            }
        }
    }
};

int main(){

    return 0;
}

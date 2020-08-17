#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "muduo/inc/mutex.h"
#include "muduo/inc/thread.h"

using namespace std;
using namespace muduo;

class Customer : Noncopyable{
private:
    mutable Mutex mutex_;
    using Entry = pair<string, int> ;
    using EntryList = vector<Entry>;
    using Map = map<string, EntryList>;
    using MapPtr = shared_ptr<Map>;
    MapPtr data_;
    
    void update(const string &customer, const EntryList& stock){
        MutexLockGuard gurad(mutex_);
        if(!data_.unique()){
            data_ = make_shared<Map>(*data_);
        }
        (*data_)[customer] = stock;
    }
    
    MapPtr get_data() const{
        MutexLockGuard gurad(mutex_);
        return data_;
    }
    int findEntry(const EntryList& entries, const string &stock) const{
        for(int i = 0; i < entries.size(); ++i){
            if(entries[i].first == stock)
                return entries[i].second;
        }
        return -1;
    }
public:
    Customer() : data_(new Map){

    }
    int query(const string &customer, const string &stock) const{
        MapPtr data = get_data();
        Map::const_iterator entries = data->find(customer);
        if(entries != data->end()){
            return findEntry(entries->second, stock);
        }else{
            return -1;
        }
    }

};

int main()
{

    cout << "yixi-test begin" << endl;


    cout << "yixi-test end" << endl;

    return 0;
}

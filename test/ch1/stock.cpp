#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>


#include "muduo/inc/mutex.h"

using namespace std;
using namespace muduo;

class Stock : public Noncopyable{
private:
    string name_;
public:
    Stock(const string &name){
        name_ = name;
    }

    string get_name() const{
        return name_;
    }
};

class StockFactory : public Noncopyable{
private :
    mutable Mutex mutex_;
    map<string, weak_ptr<Stock>> stocks_;

    void delete_stock(Stock *stock){
        if(stock){
            MutexLockGuard guard(mutex_);
            stocks_.erase(stock->get_name());
        }
        //delete stock;
    }
    static void weak_delete_stock(weak_ptr<StockFactory> wp, Stock *stock){
        shared_ptr<StockFactory> sp(wp);
        if(sp.get()){
            sp->delete_stock(stock);
        }
        delete stock;
    }

public:
    shared_ptr<Stock> get_stock(const string name, weak_ptr<StockFactory> our){
        shared_ptr<Stock> spStock;
        MutexLockGuard guard(mutex_);
        weak_ptr<Stock> &wpStock = stocks_[name];
        spStock = wpStock.lock();
        if(!spStock.get()){
            spStock.reset(new Stock(name), bind(&StockFactory::weak_delete_stock, our, placeholders::_1));
            wpStock = spStock;
        }
        return spStock;
    }
};

int main(){

    return 0;
}

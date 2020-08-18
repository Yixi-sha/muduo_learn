#ifndef __MY_NONCOPYABLE_H__
#define __MY_NONCOPYABLE_H__

#include <iostream>
using namespace std;

namespace muduo{

class Noncopyable{
public:
    Noncopyable() {}
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator = (const Noncopyable&) = delete;
};

}



#endif // __MY_NONCOPYABLE_H__ 
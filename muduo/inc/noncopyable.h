#ifndef __MY_NONCOPYABLE_H__
#define __MY_NONCOPYABLE_H__

#include <iostream>
using namespace std;

namespace muduo{

#define err(s) do{\
    cout << s << endl;\
    cout << __FILE__ <<": "<< __LINE__ << endl;\
    exit(1);\
}while(0)

class Noncopyable{
public:
    Noncopyable() {}
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator = (const Noncopyable&) = delete;
};

}



#endif // __MY_NONCOPYABLE_H__ 
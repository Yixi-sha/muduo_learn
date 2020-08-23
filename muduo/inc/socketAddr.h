#ifndef __MY_SOCKETADDR_H__
#define __MY_SOCKETADDR_H__
#include <string>

#include "object.h"


namespace muduo{
using namespace std;

class SocketAddr : public Object{
private:
    string hostname_;
    string server_;
    bool ipv6_;
    
public:
    SocketAddr();
    ~SocketAddr();
};
}


#endif
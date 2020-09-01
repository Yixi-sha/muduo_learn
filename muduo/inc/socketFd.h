#ifndef __MY_SOCKETFD_H__
#define __MY_SOCKETFD_H__

#include "noncopyable.h"
#include "socketAddr.h"
#include <memory>

namespace muduo{
using namespace std;
class SocketFd : public Object{
protected:
    int fd_;
    bool shutRead_;
    bool shutWrite_;
    bool ipv6_;
public:
    SocketFd(int fd = -1, bool ipv6 = false);
    virtual ~SocketFd() = 0;
    

    int get_fd() const;

    bool set_reuse_addr(bool on);

    bool set_noblock(bool no);
    bool is_noblock();

    bool set_cloexec(bool no);
    bool is_cloexec();

    int read(char *buf, int len);
    int write(const char *buf, int len);

    bool close();
    bool shutdown(int how);

    bool is_shutRead() const;
    bool is_shutWrite() const;

    bool is_ipv6() const;
};
}



#endif
#include "../inc/socketFd.h"
#include "../inc/wrap_func.h"

extern "C"{
#include <unistd.h>
}


namespace muduo{
SocketFd::SocketFd(bool ipv6):fd_(-1), shutRead_(false), shutWrite_(false),
ipv6_(ipv6){
}

SocketFd::~SocketFd(){
    close();
}


bool SocketFd::set_reuse_addr(bool on){
    if(fd_ < 0)
        return false;
    int opt = on ? 1 : 0;
    if(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        return false;
    return true;
}

int SocketFd::get_fd() const{
    return fd_;
}

bool SocketFd::set_noblock(bool no){
    if(fd_ < 0)
        return false;
    int nowFlags = fcntl(fd_, F_GETFL, 0);
    if(nowFlags < 0)
        return false;
    if(no){
        nowFlags |= O_NONBLOCK;
    }else{
        nowFlags &= (~O_NONBLOCK);
    }
    if(fcntl(fd_, F_SETFL, nowFlags) < 0)
        return false;
    return true;
}

bool SocketFd::is_noblock(){
      if(fd_ < 0)
        return false;
    int nowFlags = fcntl(fd_, F_GETFL, 0);
    return nowFlags & O_NONBLOCK;
}

bool SocketFd::set_cloexec(bool no){
    if(fd_ < 0)
        return false;
    int nowFlags = fcntl(fd_, F_GETFL, 0);
    if(nowFlags < 0)
        return false;
    if(no){
        nowFlags |= FD_CLOEXEC;
    }else{
        nowFlags &= (~FD_CLOEXEC);
    }
    if(fcntl(fd_, F_SETFL, nowFlags) < 0)
        return false;
    return true;
}

bool SocketFd::is_cloexec(){
    if(fd_ < 0)
        return false;
    int nowFlags = fcntl(fd_, F_GETFL, 0);
    return nowFlags & FD_CLOEXEC;
}

int SocketFd::read(char *buf, int len){
    if(fd_ == -1){
        return -1;
    }
    return Read(fd_, buf, len);

}
int SocketFd::write(char *buf, int len){
    if(fd_ == -1){
        return -1;
    }
    return Write(fd_, buf, len);
}

bool SocketFd::close(){
    if(fd_ >= 0)
        return(::close(fd_) == 0);
    else
        return false;
}

bool SocketFd::shutdown(int how){
    if(fd_ >= 0){
        if(::shutdown(fd_, how) == 0){
            if(how == SHUT_RD){
                shutRead_ = true;
            }else if(how == SHUT_WR){
                shutWrite_ = true;
            }else{
                shutRead_ = true;
                shutWrite_ = true;
            }
            if(is_shutRead() && is_shutWrite()){
                fd_ = -1;
            }
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool SocketFd::is_shutRead() const{
    return shutRead_;
}
bool SocketFd::is_shutWrite() const{
    return shutWrite_;
}

bool SocketFd::is_ipv6() const{
    return ipv6_;
}

}
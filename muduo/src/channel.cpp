#include "../inc/channel.h"

namespace muduo{

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = POLLIN | POLLPRI;
const int Channel::kWriteEvent_ = POLLOUT;
const int Channel::kErrorEvent_ = POLLERR;

}
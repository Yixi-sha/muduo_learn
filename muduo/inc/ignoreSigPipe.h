#ifndef __MY_IGNORESIGPIPE_H__
#define __MY_IGNORESIGPIPE_H__

#include "noncopyable.h"
#include "wrap_func.h"

namespace muduo{

class IgnoreSigPipe : public Noncopyable{

public:
    IgnoreSigPipe() {
        ::Signal(SIGPIPE, SIG_IGN);
    }

};

}

#endif
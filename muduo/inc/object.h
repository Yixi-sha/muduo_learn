#ifndef __MY_OBJECT_H__
#define __MY_OBJECT_H__

#include <cstdlib>

namespace muduo{

class Object{
public:
    void* operator new(unsigned long size){
        return malloc(size);
    }
    void operator delete (void* p){
        free(p);
    }
    void* operator new[] (unsigned long size){
        return malloc(size);
    }
    void operator delete[] (void* p){
        free(p);
    }
    
};

}



#endif
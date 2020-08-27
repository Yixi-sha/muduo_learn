#ifndef __MY_OBJECT_H__
#define __MY_OBJECT_H__

#include <cstdlib>
#include <memory>

namespace muduo{

using namespace std;

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
    virtual ~Object(){
    }
};

class Object_shared : public enable_shared_from_this<Object_shared>{
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
    virtual ~Object_shared(){
    }
};

}



#endif
#ifndef __MY_BUFFER_H__
#define __MY_BUFFER_H__

#include "noncopyable.h"

namespace muduo{
class Buffer : public Noncopyable{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 100;
private:
    char *buf_;
    int capacity_;
    int readerIndex_;
    int writerIndex_;

    Buffer(int capacity, int readerIndex = kCheapPrepend, int writerIndex = kCheapPrepend);
    bool construct_two();
    Buffer(const Buffer&) = delete;
    bool chang_size(int capacity);
    void reset();
public:
    static Buffer *construc_buffer(int capacity = kInitialSize);
    ~Buffer();

    void swap(Buffer& buf);
    int readable_bytes() const;
    int writable_bytes() const;
    int readable_index() const;
    const char *peek() const;
    bool retrieve(int len);
    bool retrieve_until(const char* end);
    void retrieve_all();
    string retrieveAsString();
    bool append(const char *data, size_t len);
    bool append(const string& str);
    bool append(const void *data, size_t len);
};
}



#endif
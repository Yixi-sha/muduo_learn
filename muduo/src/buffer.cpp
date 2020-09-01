#include "../inc/buffer.h"
#include "../inc/log.h"
#include "../inc/wrap_func.h"

extern "C"{
#include <strings.h>
#include <sys/uio.h>
}

namespace muduo{

Buffer::Buffer(int capacity, int readerIndex, int writerIndex) 
: capacity_(capacity), readerIndex_(readerIndex), writerIndex_(writerIndex){
    buf_ = new char[capacity];
}

bool Buffer::construct_two(){
    if(!buf_)
        return false;
    return true;
}

Buffer *Buffer::construc_buffer(int capacity){
    Buffer *ret = new Buffer(capacity + kCheapPrepend);
    if(!ret || !ret->construct_two()){
        if(ret){
            delete ret;
        }
        return nullptr;
    }
    return ret;
}

Buffer::~Buffer(){
    if(buf_)
        delete[] buf_;
}

void Buffer::swap(Buffer& buf){
    ::swap(buf_, buf.buf_);
    ::swap(readerIndex_, buf.readerIndex_);
    ::swap(writerIndex_, buf.writerIndex_);
}

int Buffer::readable_bytes() const{
    return writerIndex_ - readerIndex_;
}

int Buffer::writable_bytes() const{
    return capacity_ - writerIndex_;
}

int Buffer::readable_index() const{
    return readerIndex_;
}

const char *Buffer::peek() const{
    return buf_ + readerIndex_;
}

bool Buffer::retrieve(int len){
    if(len > readable_bytes()){
        LOG_ERROR << "Buffer::retrieve(int len)" << endl;
        return false;
    }
    readerIndex_ += len;
    if(readerIndex_ == writerIndex_){
        retrieve_all();
    }
    return true;
}

bool Buffer::retrieve_until(const char* end){
    if(end < buf_ || end >= (buf_ + capacity_)){
        LOG_ERROR << "Buffer::retrieve_until" << endl;
        return false;
    }
    return retrieve(end - peek());
}

void Buffer::retrieve_all(){
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
}

string Buffer::retrieve_as_String(){
    string ret(peek(), readable_bytes());
    retrieve_all();
    return ret;
}

void Buffer::reset(){
    if(readerIndex_ > kCheapPrepend){
        int dateNum = readable_bytes();
        char *last = buf_ + readerIndex_ + dateNum;
        char *d = buf_ + kCheapPrepend, *s = buf_ + readerIndex_;
        while(s != last){
            *d = *s;
            ++d;
            ++s;
        }
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend + dateNum;
    }
}

bool Buffer::chang_size(int capacity){
    if(capacity <= capacity_){
        reset();
        return true;
    }else{
        char *tmpbuf = new char[capacity];
        if(!tmpbuf)
            return false;
        int dateNum = readable_bytes();
        if(capacity < (dateNum + kCheapPrepend)){
            delete tmpbuf;
            return false;
        }
        ::copy(buf_ + readerIndex_, buf_ + readerIndex_ + dateNum, tmpbuf + kCheapPrepend);
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend + dateNum;
        ::swap(buf_, tmpbuf);
        capacity_ = capacity;
        delete[] tmpbuf;
        return true;
    }
}

bool Buffer::append(const char *data, size_t len){
    if(writable_bytes() < len){
        int newCapacity = capacity_ + (len - writable_bytes()) * 2;
        bool ret = chang_size(newCapacity + kCheapPrepend);
        if(!ret)
            return false;
    }else{
        reset();
    }
    ::copy(data, data + len, buf_ + writerIndex_);
    writerIndex_ += len;
    return true;
}

bool Buffer::append(const string& str){
    return append(str.c_str(), str.size());
}

bool Buffer::append(const void *data, size_t len){
    return append(reinterpret_cast<const char*>(data), len);
}

bool Buffer::ensure_writableBytes(int len){
    if(writable_bytes() < len){
        return chang_size(len + capacity_ + kCheapPrepend);
    }
    return true;
}

char* Buffer::begin_write(){
    return buf_ + writerIndex_;
}

const char* Buffer::begin_write() const{
    return buf_ + writerIndex_;
}

bool Buffer::has_written(size_t len){
    if(len + writerIndex_ <= capacity_){
        len += writerIndex_;
        return true;
    }
    return false;
}

bool Buffer::prepend(const void *data, size_t len){
    if(readerIndex_ - len < 0)
        return false;
    readerIndex_ -= len;
    const char* d = reinterpret_cast<const char*>(data);
    ::copy(d, d + len, buf_ + readerIndex_);
    return true;
}

bool Buffer::shrink(size_t reserve){
    int nowDataNum = readable_bytes();
    char *newBuf = new char[kCheapPrepend + nowDataNum + reserve];
    if(!newBuf)
        return false;

    ::copy(buf_ + readerIndex_, buf_ + readerIndex_ + nowDataNum, newBuf + kCheapPrepend);
    ::swap(buf_, newBuf);
    readerIndex_ = kCheapPrepend;
    writerIndex_ = readerIndex_ + nowDataNum;
    capacity_ = kCheapPrepend + nowDataNum + reserve;
    delete newBuf;
    return true;
}

int Buffer::read_fd(int fd, int* savedErron){
    char data[65535];
    struct iovec vec[2];
    const int writeAble = writable_bytes();

    vec[0].iov_base = buf_ + writerIndex_;
    vec[0].iov_len = writeAble;
    vec[1].iov_base = data;
    vec[1].iov_len = sizeof(data);

    int ret = ::readv(fd, vec, 2);
    if(ret < 0){
        LOG_ERROR << "Buffer::read_fd" << endl;
        *savedErron = errno;
    }else if(ret <= writeAble){
        writerIndex_ += ret;
    }else{
        writerIndex_ = capacity_;
        if(!append(data, ret - writeAble)){
            LOG_ERROR << "Buffer::read_fd !append(data, ret - writeAble)" << endl;
            return writeAble;
        }
    }
    return ret;
}

}

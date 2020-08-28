#include "../inc/buffer.h"
#include "../inc/log.h"

namespace muduo{

extern "C"{
#include <strings.h>
}


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
    if((readerIndex_ + len) > readable_bytes()){
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

string Buffer::retrieveAsString(){
    string ret(peek(), readable_bytes());
    retrieve_all();
    return ret;
}

void Buffer::reset(){
    if(readerIndex_ != kCheapPrepend){
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
    cout << capacity_ << endl;
    if(capacity <= capacity_){
        reset();
        return true;
    }else{
        char *tmpbuf = new char[capacity];
        if(!tmpbuf)
            return false;
        int dateNum = readable_bytes();
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
    int newCapacity = readable_bytes() + len;
    bool ret = chang_size(newCapacity + kCheapPrepend);
    cout << "append " <<capacity_ << endl;
    if(!ret)
        return false;
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

}
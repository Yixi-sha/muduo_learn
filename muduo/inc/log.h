#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

namespace muduo{

#define err(s) do{\
    cout << s << endl;\
    cout << __FILE__ <<": "<< __LINE__ << endl;\
    exit(1);\
}while(0)


class Log{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };
    static const char *LOG_PATH;
private:
    enum LogLevel logLevel_;
    shared_ptr<ofstream> out_;

    const char* file_;
    int line_;
    LogLevel level_;
    const char* func_;
private:
    ostream& stream_inter() const {
        if(out_.use_count() == 0){
            return cout;
        }else{
            return *out_;
        }
    }
    void out_init(const char* outPath = nullptr){
        if(outPath != nullptr){
            out_ = make_shared<ofstream>(outPath, ios::app);
            if(!out_->is_open()){
                out_.reset();
            }
        }
    }
public:
    Log(const char* file, int line, const char* outPath = nullptr):
    file_(file), line_(line), func_(nullptr), level_(TRACE){
        out_init(outPath);
    }
    Log(const char* file, int line, LogLevel level, const char* outPath = nullptr):
    file_(file), line_(line), func_(nullptr), level_(level){
        out_init(outPath);
    }
    Log(const char* file, int line, const char* func, LogLevel level, const char* outPath = nullptr):
    file_(file), line_(line), func_(func), level_(level){
        out_init(outPath);
    }
    ~Log(){

    }

    ostream& stream() const{
        ostream& ret = stream_inter();
        switch (level_)
        {
            case TRACE:
                ret << "TRACE";
                break;
            case DEBUG:
                ret << "DEBUG";
                break;
            case INFO:
                ret << "INFO";
                break;
            case WARN:
                ret << "WARN";
                break;
            case ERROR:
                ret << "ERROR";
                break;
            case FATAL:
                ret << "FATAL";
                break;    
            default:
                break;
        }
        ret << ": "<<file_ << ": " << line_ << " ";
        if(func_)
            ret << func_<<": ";
        ret <<"\t";
        return ret;
    }
    ostream& get_stream() const{
        return stream();
    }
};



#define LOG_TRACE Log(__FILE__, __LINE__, __func__, Log::TRACE, nullptr).stream()

#define LOG_DEBUG Log(__FILE__, __LINE__, __func__, Log::DEBUG, nullptr).stream()

#define LOG_INFO Log(__FILE__, __LINE__, __func__, Log::INFO, nullptr).stream()

#define LOG_WARN Log(__FILE__, __LINE__, __func__, Log::WARN, nullptr).stream()

#define LOG_ERROR Log(__FILE__, __LINE__, __func__, Log::ERROR, nullptr).stream()

#define LOG_FATAL Log(__FILE__, __LINE__, __func__, Log::FATAL, nullptr).stream()

}


#endif
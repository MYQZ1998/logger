#ifndef _LOGGER_H
#define _LOGGER_H

#include <thread>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>
#include <memory>
#include <time.h>
#include <atomic>
#include <chrono>

// windows
#include <io.h>
// Liunx
// #include <unistd.h>

namespace log
{

enum class logLevel
{
    Debug = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4
};

enum class logDst
{
    Console = 0, File = 1, All = 2
};


class tempStream : public std::ostringstream
{
private:
    friend class logger;

    std::stringstream& dst_stream;
    logLevel log_level;
    logLevel least_log_level;
    static std::mutex mt;

    tempStream(std::stringstream& dst_stream_c, logLevel log_level_c, logLevel least_log_level_c):dst_stream(dst_stream_c), log_level(log_level_c)
    , least_log_level(least_log_level_c){}
    tempStream(tempStream& ts):dst_stream(ts.dst_stream), log_level(ts.log_level), least_log_level(ts.least_log_level){}
    
public:
    ~tempStream()
    {
        std::string s = this->str();
        if(s.length() != 0)
        {
            if(log_level >= least_log_level)
            {
                time_t time_seconds = time(0);
                struct tm now_time;
                localtime_r(&time_seconds, &now_time);
                char time_str[20];
                snprintf(time_str, 20, "%d-%d-%d %d:%d:%d", now_time.tm_year + 1900, now_time.tm_mon + 1,
        now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

                mt.lock();
                switch(log_level)
                {
                    case logLevel::Debug:
                    {
                        dst_stream << "[Debug]";
                        break;
                    }
                    case logLevel::Info:
                    {
                        dst_stream << "[Info]";
                        break;
                    }
                    case logLevel::Warning:
                    {
                        dst_stream << "[Warning]";
                        break;
                    }
                    case logLevel::Error:
                    {
                        dst_stream << "[Error]";
                        break;
                    }
                    case logLevel::Fatal:
                    {
                        dst_stream << "[Fatal]";
                        break;
                    }
                }
                dst_stream << time_str << std::endl;
                dst_stream << this->str() << std::endl;
                mt.unlock();
            }
        }
    }
};

class logger
{
private:
    // friend class
    friend class tempStream;

    // single instance
    static logger lgr;

    // buffer
    std::stringstream console_stream;
    std::stringstream file_stream;

    // dst folder
    std::string dst_txt;

    // log level
    logLevel log_level;

    // lock
    std::mutex logger_lock;
    std::condition_variable logger_variable;

    // control flag
    std::atomic<bool> if_stop;
    std::thread flush_thread;
    // flush time
    std::atomic<int> time;

    logger():log_level(logLevel::Debug), dst_txt("./log.txt"), time(1000), if_stop(false)
    {
        this->console_stream.clear();
        this->file_stream.clear();
        this->flush_thread = std::thread([this]()
        {
            while(!this->if_stop)
            {
                std::unique_lock<std::mutex> ul(this->logger_lock);
                this->logger_variable.wait_for(ul, std::chrono::milliseconds(this->time));
                ul.unlock();
                this->flush();
            }
        });
    }

public:
    ~logger()
    {
        this->if_stop = true;
        this->time = 0;
        this->logger_variable.notify_all();
        this->flush_thread.join();
    }

    logger(logger&) = delete;

    // get single instance
    static logger& get_logger();
    // get log level
    logLevel get_log_level();
    // set log level
    logLevel set_log_level(logLevel new_log_level);
    // overload operator ()
    tempStream operator ()(logDst log_dst, logLevel log_level = logLevel::Debug);
    // flush
    void flush(logDst log_dst = logDst::All);
    // set flush time
    void set_flush_time(int new_time);
    // get flush time
    int get_flush_time();
    // set destination folder
    bool set_dst_txt(std::string txt_path);
    // get destination folder
    std::string get_dst_txt();
};

}

#endif
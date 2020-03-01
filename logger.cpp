#include "logger.h"
using namespace log;

std::mutex tempStream::mt;
logger logger::lgr;

logger& logger::get_logger()
{
    return lgr;
}

logLevel logger::get_log_level()
{
    this->logger_lock.lock();
    logLevel ret = this->log_level;
    this->logger_lock.unlock();
    return ret;
}

logLevel logger::set_log_level(logLevel new_log_level)
{
    this->logger_lock.lock();
    this->log_level = new_log_level;
    logLevel ret = this->log_level;
    this->logger_lock.unlock();
    return ret;
}

tempStream logger::operator ()(logDst log_dst, logLevel log_level)
{
    switch(log_dst)
    {
        case logDst::Console:
        {
            tempStream temp_stream(this->console_stream, log_level, this->log_level);
            return temp_stream;
        }
        case logDst::File:
        {
            tempStream temp_stream(this->file_stream, log_level, this->log_level);
            return temp_stream;
        }
    }
}

void logger::flush(logDst log_dst)
{
    switch(log_dst)
    {
        case logDst::Console:
        {
            this->logger_lock.lock();
            std::cout << console_stream.str();
            this->console_stream.clear();
            this->logger_lock.unlock();
            break;
        }
        case logDst::File:
        {
            this->logger_lock.lock();
            std::ofstream out;
            out.open(this->dst_txt, std::ios::app);
            if(out)
            {
                out << this->file_stream.str();
            }
            this->file_stream.clear();
            this->logger_lock.unlock();
            break;
        }
        case logDst::All:
        {
            this->logger_lock.lock();

            if(this->console_stream.str().length() != 0)
            {
                std::cout << this->console_stream.str();
                this->console_stream.clear();
            }

            std::ofstream out;
            out.open(this->dst_txt, std::ios::app);
            if(out)
            {
                out << this->file_stream.str();
            }
            this->file_stream.clear();
            this->logger_lock.unlock();
            break;
        }
    }
}

void logger::set_flush_time(int new_time)
{
    this->time = new_time;
}

int logger::get_flush_time()
{
    return this->time;
}

bool logger::set_dst_txt(std::string txt_path)
{
    if(access(txt_path.c_str(), 4))
    {
        this->logger_lock.lock();
        this->dst_txt = txt_path;
        this->logger_lock.unlock();
        return true;
    }
    else
        return false;
}

std::string logger::get_dst_txt()
{
    this->logger_lock.lock();
    std::string ret = this->dst_txt;
    this->logger_lock.unlock();
    return ret;
}
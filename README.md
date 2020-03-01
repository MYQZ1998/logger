# logger
基于C++11标准实现的线程安全的日志系统  
使用了饿汉单例模式，对所有线程提供唯一的日志器（logger）  
命名空间 log  
***  
## 类接口函数  
**static logger& get_logger()**  
返回日志器的引用  
  
**logLevel get_log_level()**  
获取目前日志器的记录等级  
  
**logLevel set_log_level(logLevel new_log_level)**  
设置日志器的记录等级  
  
**tempStream operator ()(logDst log_dst, logLevel log_level = logLevel::Debug)**  
根据输出目标返回流的暂时对象，设置此次的记录等级  

**void flush(logDst log_dst = logDst::All)**  
立即将日志器缓冲区里的内容写入到目标区  
  
**void set_flush_time(int new_time)**  
设置日志器自动 flush 的时间间隔，单位为毫秒  
  
**int get_flush_time()**  
获取日志器自动 flush 的时间间隔，单位为毫秒  
  
**bool set_dst_txt(string txt_path)**  
设置 logDst::File 的输出目标文件，需要是可写文本的文件  
成功返回true  
失败返回false  
  
**string get_dst_txt()**  
获取 logDst::File 的输出目标文件
***  
## 日志记录等级  
使用C++的强枚举类型编写，如下：  
enum class logLevel  
{  
    Debug = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4  
}  
  
当想要写入的日志等级小于日志器设置的日志等级时，该日志消息不会被记录  
***  
## 日志输出类型  
同样使用C++的强枚举类型编写，如下：  
enum class logDst  
{  
    Console = 0, File = 1, All = 2  
}  
Console 代表输出到控制台，File 代表输出到文件。  
All 目前只在 flush 函数中用到。  
***  
## 使用例子  
logger& lgr = logger::get_logger();  
for(int j = 0; j < 100; j++)  
    lgr(logDst::Console) << j;  
#pragma once
#include <string>
#include "lockqueue.h"

/*
异步写日志
1. 多个工作线程向m_lockQue中写入数据
2. 一个日志线程将m_lockQue中的数据写入文件
*/

enum Level
{
    INFO,
    ERROR,
};

class Logger
{
public:
    static Logger& GetInstance();
    void setLevel(Level lev);
    void Log(std::string msg);

private:
    int m_level;
    Lockqueue<std::string> m_lockQue;

    // 单例模式
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

// 定义宏 LOG_INFO("日志内容：%d %s", 10, str_info);
#define LOG_INFO(logmsgformat, ...) \
    do  \
    {   \
        Logger &logger = Logger::GetInstance(); \
        logger.setLevel(INFO);  \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);  \
    } while (0);

#define LOG_ERR(logmsgformat, ...) \
    do  \
    {   \
        Logger &logger = Logger::GetInstance(); \
        logger.setLevel(ERROR);  \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);  \
    } while (0);

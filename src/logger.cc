#include "logger.h"

#include <time.h>
#include <thread>
#include <iostream>

Logger::Logger() {
    // 启动专门写日志的线程
    std::thread writeLogTask([&]() {
        for(; ;) {
            // 获取当前的日期，把日志信息写入相应的文件中 追加模式(a+)
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "logger file: " << file_name << "open error! " << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lockQue.Pop();

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    // 分离线程
    writeLogTask.detach();
}

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

void Logger::setLevel(Level lev) {
    m_level = lev;
}

// 写日志，放入队列缓冲区中
void Logger::Log(std::string msg) {
    // 把日志信息前加入时间信息
    time_t now = time(nullptr);
    tm *nowtm = localtime(&now);
    char time_buf[128] = {0};
    sprintf(time_buf, "[ %s %d:%d:%d ] ", 
        (m_level == INFO ? "INFO" : "ERROR"),
        nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
    msg.insert(0, time_buf);
    // 行尾加上换行符
    msg.append("\n");
    m_lockQue.Push(msg);
}
#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // 启动zkserver
    void Start();
    // 在server上根据指定的path创建znode节点
    void Create(const char* path, const char* data, int datalen, int state = 0);
    // 根据指定的路径，获取znode节点的值
    std::string GetData(const char* path);

private:
    // zk客户端的句柄
    zhandle_t* m_zhandle;
};
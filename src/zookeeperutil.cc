#include "zookeeperutil.h"
#include "rpcapplication.h"
#include "logger.h"

ZkClient::ZkClient() : m_zhandle(nullptr) {

}
ZkClient::~ZkClient() {
    if (m_zhandle != nullptr) {
        // 关闭句柄
        zookeeper_close(m_zhandle);
    }
}

// 回调函数单独在一个线程
void global_watcher(zhandle_t *zh, int type, 
                    int state, const char *path, void *watcherCtx) {
    // 会话相关的消息类型
    if (type == ZOO_SESSION_EVENT) {
        // 连接成功
        if (state == ZOO_CONNECTED_STATE) {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            // 唤醒主线程
            sem_post(sem);
        }
    }
}

// 连接zk Server
void ZkClient::Start() {
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string conninfo = host + ":" + port;

    /*
    zookeeper_mt 多线程版本
    zk client 三个线程
        1 当前调用线程
        2 网络IO线程
        3 watcher回调线程
    */
    m_zhandle = zookeeper_init(conninfo.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr) {
        std::cout << "zookeeper init fail!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // zk句柄创建成功，等待连接建立，调用回调函数内的sem_post
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper init success!");
}

// 在server上根据指定的path创建znode节点
void ZkClient::Create(const char* path, const char* data, int datalen, int state) {
    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    // 不存在 才创建
    if(ZNONODE == flag) {
        char path_buffer[128];
        int buflen = sizeof(path_buffer);
        flag = zoo_create(m_zhandle, path, data, datalen, 
                &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, buflen);
        if (flag == ZOK) {
            LOG_INFO("znode create success: %s !", path);
        }
        else {
            LOG_ERR("flag: %d", flag);
            LOG_ERR("znode create fail, path: %s", path);
            exit(EXIT_FAILURE);
        }
    }

}

// 根据指定的路径，获取znode节点的值
std::string ZkClient::GetData(const char* path) {
    char buf[64] = {0};
    int buflen =sizeof(buf);
    int flag = zoo_get(m_zhandle, path, 0, buf, &buflen, nullptr);
    if (flag == ZOK) {
        return buf;
    }
    else {
        LOG_ERR("flag: %d", flag);
        LOG_ERR("znode get fail, path: %s", path);
        exit(EXIT_FAILURE);
    }
}
#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <google/protobuf/descriptor.h>
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

#include "rpcapplication.h"


// 框架中负责发布rpc服务的网络对象类
class RpcProvider
{
public:
    // 发布rpc方法的函数接口
    void NotifyServeice(google::protobuf::Service* service);

    // 启动rpc服务服务节点
    void Run();

private:
    // 组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    // 服务类型的信息:服务对象；服务包含的方法map
    struct ServiceInfo
    {
        google::protobuf::Service *m_service; // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_method_map; // 服务方法
    };

    // 存储{服务名，服务相关信息}
    std::unordered_map<std::string, ServiceInfo> m_service_map;

    // 新的连接回调
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    // 读写事件回调
    void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    // Closure回调，用于序列化rpc响应和回送消息
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *);
};
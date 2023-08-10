/*
muduo库的使用示例：

muduo两个主要类
TcpServer 服务器程序
TcpClient 客户端程序

epoll + 线程池
使用muduo的好处：
    能够把I/O代码和业务代码区分开
    只需要关心两种状态 —— 用户的连接和断开；用户的可读写事件
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*
基于muduo库开发服务器程序
1 组合TcpServer对象
2 创建EventLoop事件循环指针
3 明确TcpServer构造函数需要什么参数，写出对应的ChatServer的构造函数
4 在当前服务器类的构造函数中，注册回调函数（处理连接和读写事件）
5 设置服务端线程数量，muduo会自行分配I/O线程和worker线程
*/

class ChatServer
{
public:
    ChatServer(
        EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg)
        :_server(loop, listenAddr, nameArg),
        _loop(loop) {
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量
        _server.setThreadNum(4);
    }

    void start() {
        _server.start();
    }

private:
    // 处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " stste:online" << endl;
        } else {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " stste:offline" << endl;
            conn->shutdown();
        }
        }

    void onMessage(
        const TcpConnectionPtr &conn,
        Buffer *buf,
        Timestamp time) 
    {
        string mes = buf->retrieveAllAsString();
        cout << "recv data:" << mes << " time:" << time.toString() << endl;
        conn->send(mes);
    }
    TcpServer _server;
    EventLoop *_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    // 启动服务：listenfd监听
    server.start();
    // epoll_wait以阻塞方式等待新用户连接，或者用户读写事件
    loop.loop();

    return 0;
}

// g++ -std=c++11 -o server muduo_server.cpp -lmuduo_net -lmuduo_base -lpthread
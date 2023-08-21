#include <iostream>
#include <string>
#include "user.pb.h"
#include "rpcprovider.h"
#include "rpcapplication.h"

using namespace fixbug;

// 提供本地服务
class UserService : public UserServiceRpc
{
public:
    bool Login(std::string& name, std::string& pwd) {
        std::cout << "Doing local service: login" << std::endl;
        std::cout << "name:" << name << "  password:" << pwd << std::endl;
        return true;
    }
    /*
        重写 UserServiceRpc 的虚函数，框架直接调用这些方法
        服务提供方的框架给业务上报请求参数LoginRequest，应用获取数据后做本地业务
    */
    void Login(::google::protobuf::RpcController *controller,
                const ::fixbug::LoginRequest *request,
                ::fixbug::LoginResponse *response,
                ::google::protobuf::Closure *done) 
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Login(name, pwd);

        fixbug::ResultCode* rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(ret);

        // 回调，响应消息的序列化和网络发送
        done->Run();

    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "Doing local service: Register" << std::endl;
        std::cout << "id: " << id << "name:" << name << "  password:" << pwd << std::endl;
        return true;
    }

    void Register(::google::protobuf::RpcController* controller,
                        const ::fixbug::RegisterRequest* request,
                        ::fixbug::RegisterResponse* response,
                        ::google::protobuf::Closure* done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id, name, pwd);

        fixbug::ResultCode* rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(ret);

        // 回调，响应消息的序列化和网络发送
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 框架初始化
    MprpcApplication::Init(argc, argv);

    // rpc网络服务对象
    RpcProvider provider;
    provider.NotifyServeice(new UserService());

    // 等待远程的rpc调用请求
    provider.Run();

    return 0;
}
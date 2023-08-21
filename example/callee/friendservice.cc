#include <iostream>
#include <string>
#include "friend.pb.h"
#include "rpcprovider.h"
#include "rpcapplication.h"
#include "logger.h"

using namespace fixbug;

class FriendService : public FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendList(uint32_t id) {
        std::cout << "Get Friend List !" << std::endl;
        std::vector<std::string> vec;
        vec.push_back("f1 name"); 
        vec.push_back("f2 name");
        vec.push_back("f3 name");
        return vec;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                        const ::fixbug::GetFriendListRequest* request,
                        ::fixbug::GetFriendListResponse* response,
                        ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendList = GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (auto &name : friendList) {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }

};

int main(int argc, char **argv)
{
    LOG_INFO("first log message!");
    LOG_INFO("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);

    // 框架初始化
    MprpcApplication::Init(argc, argv);

    // rpc网络服务对象
    RpcProvider provider;
    provider.NotifyServeice(new FriendService());

    // 等待远程的rpc调用请求
    provider.Run();

    return 0;
};
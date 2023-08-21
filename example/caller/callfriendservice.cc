#include <iostream>

#include "rpcapplication.h"
#include "friend.pb.h"


int main(int argc, char * argv[])
{
    // 初始化框架，只初始化一次
    MprpcApplication::Init(argc, argv);

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

    // rpc请求体
    fixbug::GetFriendListRequest req;
    req.set_userid(1110);
    // rpc响应对象
    fixbug::GetFriendListResponse resp;

    // 定义一个控制对象，传入框架用于记录调用的状态
    MprpcController contrler;
    // 调用远程发布的rpc方法login
    stub.GetFriendList(&contrler, &req, &resp, nullptr);
    // 检查调用状态，如果调用失败，则无合法的resp可访问
    if (contrler.Failed()) {
        std::cout << contrler.ErrorText();
    }
    else {
        if (0 == resp.result().errcode()) {
            std::cout << "rpc GetFriendList response success!" << std::endl;
            // 打印好友列表
            int size = resp.friends_size();
            for (int i = 0; i < size; ++i) {
                std::cout << i + 1 << ": name: " << resp.friends(i) << std::endl;
            }
        } else {
            std::cout << "rpc GetFriendList response fail !!!" << resp.result().errmsg() << std::endl;
        }
    }



    

    return 0;
}
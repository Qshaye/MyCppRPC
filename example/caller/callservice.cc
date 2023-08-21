#include <iostream>

#include "rpcapplication.h"
#include "user.pb.h"

int main(int argc, char * argv[])
{
    // 初始化框架，只初始化一次
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    // rpc请求体
    fixbug::LoginRequest req;
    req.set_name("zhangSan");
    req.set_pwd("123");

    // rpc响应内容
    fixbug::LoginResponse resp;

    // 调用远程发布的rpc方法login
    stub.Login(nullptr, &req, &resp, nullptr);

    if (0 == resp.result().errcode()) {
        std::cout << "rpc login response success!" << resp.success() << std::endl;
    } else {
        std::cout << "rpc login response fail !!!" << resp.result().errmsg() << std::endl;
    }

    // rpc调用远程的Register方法示例
    fixbug::RegisterRequest reg_req;
    reg_req.set_id(1129);
    reg_req.set_name("rpc调用Register!");
    reg_req.set_pwd("123");

    fixbug::RegisterResponse reg_rs;

    stub.Register(nullptr, &reg_req, &reg_rs, nullptr);

    if (0 == reg_rs.result().errcode()) {
        std::cout << "rpc Register response success!" << reg_rs.success() << std::endl;
    } else {
        std::cout << "rpc Register response fail !!!" << reg_rs.result().errmsg() << std::endl;
    }

    

    return 0;
}
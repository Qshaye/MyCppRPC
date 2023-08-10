#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123"); 

    // protobuf对象 序列化
    std::string send_str;
    if (req.SerializeToString(&send_str)) {
        std::cout << send_str.c_str() << std::endl;
    }

    // protobuf对象 反序列化
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str)) {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    LoginResponse rsp;
    // 嵌套的message必须使用mutable_xxx接口拿出来赋值
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("登录失败了！");

    return 0;
}
// g++ -std=c++11 proc_test_1.cc test.pb.cc -lprotobuf -lpthread
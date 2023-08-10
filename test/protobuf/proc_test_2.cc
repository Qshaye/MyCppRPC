#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    GetFriendListResponse rsp;

    // 嵌套message类型
    // 需要用 mutable_xxx 方法取出
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friendlist();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    // 列表类型 xxx
    // add_xxx方法增加元素
    User *user2 = rsp.add_friendlist();
    user2->set_name("li ming");
    user2->set_age(21);
    user2->set_sex(User::MAN);

    std::cout << rsp.friendlist_size() << std::endl;
    return 0;
}
# set(SRC_LIST callservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

add_executable(consumer ${SRC_LIST})
# 链接动态库
target_link_libraries(consumer mprpc protobuf)
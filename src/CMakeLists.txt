# 添加编译文件
# aux_source_directory(. SRC_LIST)
set(SRC_LIST 
    rpcapplication.cc 
    config.cc 
    rpcheader.pb.cc 
    rpcprovider.cc 
    rpcchannel.cc 
    rpccontroller.cc
    logger.cc
    zookeeperutil.cc)
# 将mprpc编译成静态库 （中间加上SHARED则编译为动态链接库）
add_library(mprpc ${SRC_LIST})
# 添加依赖库：muduo库 被依赖的在后面
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)


#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "rpc_channel.h"
#include "rpcheader.pb.h"
#include "rpc_application.h"
#include "zookeeperutil.h"

// 约定的请求格式：header_size service_name method_name args_size args
// 通过stub调用的rpc方法，都调用channel->CallMethod方法（数据序列化和网络发送）
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller,
                            const google::protobuf::Message *request,
                            google::protobuf::Message *response,
                            google::protobuf::Closure *done)
{
    // 根据MethodDescriptor获取它所属的服务 ServiceDescriptor
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    uint32_t args_size = 0;
    std::string args;
    if (request->SerializeToString(&args)) {
        args_size = args.size();
    } else {
        // 解析参数失败，记录到controller
        controller->SetFailed("serialize args fail!");
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader header;
    header.set_servece_name(service_name);
    header.set_method_name(method_name);
    header.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string header_str;
    if (header.SerializeToString(&header_str)) {
        header_size = header_str.size();
    } else {
        controller->SetFailed("serialize header fail!");
        return;
    }

    // 组织要发送的请求字符串
    std::string send_rpc_str;
    // 把int类型的4个字节转换为字符加入到头部
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str += header_str;
    send_rpc_str += args;

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header: "<<header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args<<std::endl;
    std::cout<<"======================================"<<std::endl;

    // tcp 编程，完成远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd) {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error, errno: %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverip").c_str());

    // 连接zk服务器，获取提供RPC服务的ip地址和端口号
    ZkClient zkcli;
    zkcli.Start();
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkcli.GetData(method_path.c_str());
    if (host_data == "") {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    // 127.0.0.1:8000
    int idx = host_data.find(":");
    if (idx == -1) {
        controller->SetFailed(method_path + " is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1).c_str());

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(port);
    ser_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc请求
    if (-1 == connect(clientfd,  (struct sockaddr*)&ser_addr, sizeof(ser_addr))) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect to server fail! errno:  %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // std::cout<<send_rpc_str.size()<<std::endl;   -》44
    // std::cout<<sizeof(send_rpc_str)<<std::endl;  -》8
    // 发送rpc请求，字符串大小要用成员变量send_rpc_str.size()，不能使用sizeof
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send req fail! errno:  %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 接收rpc响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv data fail! errno:  %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化相应内容
    // std::string response_str(recv_buf, 0, recv_size);
    // if (!response->ParseFromString(response_str)) {
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse fail! raw response:   %s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }
    close(clientfd);
}
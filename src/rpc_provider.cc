#include "rpcprovider.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

// 发布所有从Service类继承而来的具体服务实现
/*
参数：一个Service对象的指针
创建一个ServiceInfo结构体保存 指针,map<方法名,方法描述符>
在框架的成员变量m_service_map中，加入 <服务名,ServiceInfo结构体>

远程rpc请求到达
1. 先根据服务名找到ServiceInfo结构体，取出其中的Service对象
2. 根据方法名在ServiceInfo结构体中的method map表中找到对应的方法描述符
...解析参数进行调用
*/
void RpcProvider::NotifyServeice(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    service_info.m_service = service;

    // 获取传入的服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    std::string service_name = pserviceDesc->name();

    // std::cout << "发布服务: " << service_name << std::endl;
    LOG_INFO("发布服务：%s", service_name.c_str());

    int method_cnt = pserviceDesc->method_count();
    for (int i = 0; i < method_cnt; ++i) {
        const google::protobuf::MethodDescriptor *pmthodDesc = pserviceDesc->method(i);
        std::string method_name = pmthodDesc->name();
        service_info.m_method_map.insert({method_name, pmthodDesc});

        // std::cout << "method name:" << method_name << std::endl;
        LOG_INFO("服务包含方法：%s", method_name.c_str());

    }
    m_service_map.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    // 构造muduo网络库需要的地址，类型为InetAddress
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverip").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 创建zkclient连接zkserver，把当前rpc节点上发布的服务注册到zkserver上，用于服务发现
    ZkClient zkcli;
    zkcli.Start();

    for (auto &sev : m_service_map) {
        std::string path = "/" + sev.first;
        zkcli.Create(path.c_str(), nullptr, 0);
        for (auto &mt : sev.second.m_method_map) {
            std::string method_path = path + "/" + mt.first;
            char method_info[128] = {0};
            // 节点存储的信息为 ip：port
            sprintf(method_info, "%s:%d", ip.c_str(), port);
            // 注册临时节点 ZOO_EPHEMERAL
            zkcli.Create(method_path.c_str(), method_info, strlen(method_info), ZOO_EPHEMERAL);
        }
    }

    LOG_INFO("Rpc Provider start!");

    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected()) {
        conn->shutdown();
    }
}

/*
在框架内部，Rpc provider和consumer要协商好通信的数据类型
    service_name
    method_name
    args
定义相应的proto的message类型，进行数据头的序列化和反序列化

header_size(4字节) header args_size args
*/
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp)
{
    // 从网络上接受的远程rpc请求信息
    std::string recv_buf = buffer->retrieveAllAsString();


    // 从字符流中读取前四个字节的内容，作为header size
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);
    std::cout<<"send_rpc_str: "<<recv_buf<<std::endl;

    // 根据header_size读取数据头的原始字符流
    std::string rpc_header = recv_buf.substr(4, header_size);
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    mprpc::RpcHeader RpcHeader;
    if (RpcHeader.ParseFromString(rpc_header))
    {
        // 反序列化成功
        service_name = RpcHeader.servece_name();
        method_name = RpcHeader.method_name();
        args_size = RpcHeader.args_size();
    }
    else {
        std::cout << "rpc_header parse error!" << std::endl;
        return;
    }
    std::string args = recv_buf.substr(4 + header_size, args_size);

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header: "<<rpc_header<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args<<std::endl;
    std::cout<<"======================================"<<std::endl;

    // 获取service对象和method对象
    auto it = m_service_map.find(service_name);
    if (it == m_service_map.end())
    {
        std::cout << service_name << "not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_method_map.find(method_name);
    if (mit == it->second.m_method_map.end())
    {
        std::cout << method_name << "not exist!" << std::endl;
        return;
    }

    // 服务存在，取出服务对象
    google::protobuf::Service *service = it->second.m_service;
    // 获取 method Descriptor
    const google::protobuf::MethodDescriptor *method = mit->second;

    // 获取服务中的某个方法的请求类型，以将接收的二进制字符串进行解析
    google::protobuf::Message *req = service->GetRequestPrototype(method).New();
    if (!req->ParseFromString(args))
    {
        std::cout << "request args parse error!" << std::endl;
    }
    google::protobuf::Message *resp = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, 
                                    const muduo::net::TcpConnectionPtr &, 
                                    google::protobuf::Message*>
                                    (this, &RpcProvider::SendRpcResponse, conn, resp);

    // 框架根据解析远端rpc请求，调用对应的方法
    service->CallMethod(method, nullptr, req, resp, done);
}

//  序列化相应，通过网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    } else {
        std::cout << "Serialize response error!" << std::endl;
    }
    conn->shutdown();
}
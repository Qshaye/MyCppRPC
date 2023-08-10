#include "rpcapplication.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// 初始化静态成员变量
MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configFile>" << std::endl;
}

// 从配置文件读取信息，初始化
void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 1;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "serverip " << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "serverport " << m_config.Load("rpcserverport") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig &MprpcApplication::GetConfig()
{
    return m_config;
}
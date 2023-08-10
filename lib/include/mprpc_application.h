#pragma once

#include "config.h"
#include "rpcchannel.h"
#include "rpccontroller.h"

// 框架基础类，负责框架的初始化操作
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig &GetConfig();

private:
    static MprpcConfig m_config;

    MprpcApplication() {}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};
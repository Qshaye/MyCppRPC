#pragma once
#include <iostream>
#include <unordered_map>

class MprpcConfig
{
public:
    void LoadConfigFile(const char *config_file);
    std::string Load(const std::string& key);

private:
    std::unordered_map<std::string, std::string> m_config_map;
    // 去掉字符串两边的空格
    void Trim(std::string &src_buf);
};
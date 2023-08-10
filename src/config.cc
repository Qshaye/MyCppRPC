#include "config.h"
#include <string>

void MprpcConfig::LoadConfigFile(const char* config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (nullptr == pf)
    {
        std::cout << config_file << "is not exists!" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去掉多余空格
        std::string src_buf(buf);
        Trim(src_buf);

        // 解析配置项
        int idx = src_buf.find("=");
        if (idx == -1) {
            continue;
        }

        std::string key, value;
        key = src_buf.substr(0, idx);
        Trim(key);

        int end_idx = src_buf.find('\n', idx);
        value = src_buf.substr(idx + 1, end_idx - idx - 1);
        Trim(value);

        m_config_map.insert({key, value});
    }
}

std::string MprpcConfig::Load(const std::string &key)
{
    if (m_config_map.find(key) != m_config_map.end()) {
        return m_config_map[key];
    }
    return "";
}

void MprpcConfig::Trim(std::string &src_buf)
{
    // 去掉多余空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}
#pragma once

#include <string>
#include <unordered_map>



/**
框架读取配置文件类
rpcserver-ip
*/
class MprpcConfig {
public:
    void loadConfigFile(const char *config_file);
    std::string load(const std::string &key);
private:
    std::unordered_map<std::string, std::string> m_mp;
};
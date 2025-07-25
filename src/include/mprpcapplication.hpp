#pragma once

// mprpc框架的初始化类
#include "mprpcconfig.hpp"
class MprpcApplication {
public:
    static void init(int argc, char **argv);
    static MprpcApplication& getInstance();
    static MprpcConfig& getConfig();
private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
    static MprpcConfig m_config;
};
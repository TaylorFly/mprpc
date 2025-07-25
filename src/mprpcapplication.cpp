#include "mprpcapplication.hpp"
#include "mprpcconfig.hpp"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::m_config; 

void showArgsHelp() {
    std::cout << "format: command -i <configfile>\n";
}

MprpcApplication& MprpcApplication::getInstance() {
    static MprpcApplication application;
    return application; 
} 

void MprpcApplication::init(int argc, char **argv) {
    if (argc < 2) {
        showArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        if (c == 'i') {
            config_file = optarg;
        } else if (c == '?') {
            showArgsHelp();
            exit(EXIT_FAILURE);
        } else if (c == ':') {
            showArgsHelp();
            exit(EXIT_FAILURE);
        } else {
            break;
        }
    }

    //开始加载配置文件
    m_config.loadConfigFile(config_file.c_str());
}

MprpcConfig &MprpcApplication::getConfig() {
    return m_config;
}
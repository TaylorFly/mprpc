#include "mprpcconfig.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <assert.h>

void MprpcConfig::loadConfigFile(const char *config_file) {
    FILE *fp = fopen(config_file, "r");
    if (fp == nullptr) {
        std::cerr << "Config File is invalid\n";
        exit(EXIT_FAILURE);
    }
    char buf[1024];
    while (fgets(buf, 1024, fp) != nullptr) {
        std::string s = buf;
        std::string key, value;
        if (s.back() == '\n') s.pop_back();
        int n = s.size();
        std::string t;
        for (int i = 0; i < n; i++) {
            if (s[i] != ' ') {
                t += s[i];
            }
        }
        n = t.size();
        if (n == 0) continue;
        assert(t.find(' ') == -1);
        if (t[0] == '#') {
            continue;
        }

        int ep = t.find('=');
        if (ep == -1) {
            exit(EXIT_FAILURE);
        }
        key = t.substr(0, ep);
        value = t.substr(ep + 1, n - ep - 1);
        m_mp.insert({key, value});
    }
    fclose(fp);
}

std::string MprpcConfig::load(const std::string &key) {
    auto it = m_mp.find(key);
    if (it == m_mp.end()) {
        return "";
    }
    return it->second;
}


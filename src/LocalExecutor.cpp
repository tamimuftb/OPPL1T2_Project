#include "LocalExecutor.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

std::pair<std::string,std::string>
LocalExecutor::executeCommand(const std::string& cmd)
{
    std::string out, err;
    std::string tmp = "/tmp/.sr_err_" + std::to_string(reinterpret_cast<uintptr_t>(this));
    std::string full = cmd + " 2>" + tmp;

    FILE* pipe = popen(full.c_str(), "r");
    if (!pipe) return {"", "popen() failed"};

    std::array<char,256> buf{};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        out += buf.data();
    pclose(pipe);

    std::ifstream ef(tmp);
    if (ef.is_open()) {
        std::string ln;
        while (std::getline(ef, ln)) err += ln + "\n";
        ef.close();
        std::remove(tmp.c_str());
    }
    return {out, err};
}

bool LocalExecutor::uploadFile(const std::string& local, const std::string& remote)
{
    std::ifstream s(local,  std::ios::binary);
    std::ofstream d(remote, std::ios::binary);
    if (!s || !d) return false;
    d << s.rdbuf();
    return true;
}

bool LocalExecutor::downloadFile(const std::string& remote, const std::string& local)
{
    return uploadFile(remote, local);
}

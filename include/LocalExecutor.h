#pragma once
#include "IExecutor.h"
#include <string>

class LocalExecutor : public IExecutor {
public:
    LocalExecutor()  = default;
    ~LocalExecutor() = default;
    bool connect()           override { return true; }
    void disconnect()        override {}
    bool isConnected() const override { return true; }
    std::pair<std::string,std::string> executeCommand(const std::string& cmd) override;
    bool uploadFile(const std::string& local, const std::string& remote) override;
    bool downloadFile(const std::string& remote, const std::string& local) override;
};

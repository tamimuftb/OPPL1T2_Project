#pragma once
#include <string>
#include <utility>

class IExecutor {
public:
    virtual ~IExecutor() = default;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::pair<std::string,std::string> executeCommand(const std::string& cmd) = 0;
    virtual bool uploadFile(const std::string& local, const std::string& remote) = 0;
    virtual bool downloadFile(const std::string& remote, const std::string& local) = 0;
};

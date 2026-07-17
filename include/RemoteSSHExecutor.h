#pragma once
#include "IExecutor.h"
#include <string>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class RemoteSSHExecutor : public IExecutor {
public:
    RemoteSSHExecutor(const std::string& host, const std::string& user,
                      const std::string& pass, int port = 22);
    ~RemoteSSHExecutor() override;
    bool connect()           override;
    void disconnect()        override;
    bool isConnected() const override { return m_connected; }
    std::pair<std::string,std::string> executeCommand(const std::string& cmd) override;
    bool uploadFile(const std::string& local, const std::string& remote) override;
    bool downloadFile(const std::string& remote, const std::string& local) override;
private:
    std::string      m_host, m_user, m_pass;
    int              m_port;
    int              m_sock    = -1;
    LIBSSH2_SESSION* m_session = nullptr;
    LIBSSH2_SFTP*    m_sftp    = nullptr;
    bool             m_connected = false;
    void logErr(const std::string& ctx) const;
};

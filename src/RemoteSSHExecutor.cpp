#include "RemoteSSHExecutor.h"
#include <iostream>
#include <fstream>
#include <cstring>

RemoteSSHExecutor::RemoteSSHExecutor(const std::string& host, const std::string& user,
                                     const std::string& pass, int port)
    : m_host(host), m_user(user), m_pass(pass), m_port(port)
{
    if (libssh2_init(0) != 0)
        std::cerr << "[SSH] libssh2_init failed\n";
}

RemoteSSHExecutor::~RemoteSSHExecutor() { disconnect(); libssh2_exit(); }

bool RemoteSSHExecutor::connect()
{
    if (m_connected) return true;

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == -1) { std::cerr << "[SSH] socket: " << strerror(errno) << "\n"; return false; }

    sockaddr_in sin{};
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(static_cast<uint16_t>(m_port));
    sin.sin_addr.s_addr = inet_addr(m_host.c_str());

    if (::connect(m_sock, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) != 0) {
        std::cerr << "[SSH] connect to " << m_host << ":" << m_port << " failed: " << strerror(errno) << "\n";
        close(m_sock); m_sock = -1; return false;
    }

    m_session = libssh2_session_init();
    if (!m_session) { close(m_sock); m_sock = -1; return false; }

    if (libssh2_session_handshake(m_session, m_sock) != 0) {
        logErr("handshake"); libssh2_session_free(m_session); m_session = nullptr;
        close(m_sock); m_sock = -1; return false;
    }

    if (libssh2_userauth_password(m_session, m_user.c_str(), m_pass.c_str()) != 0) {
        logErr("auth"); libssh2_session_disconnect(m_session, "auth fail");
        libssh2_session_free(m_session); m_session = nullptr;
        close(m_sock); m_sock = -1; return false;
    }

    m_sftp = libssh2_sftp_init(m_session);
    if (!m_sftp) {
        logErr("sftp_init"); libssh2_session_disconnect(m_session, "sftp fail");
        libssh2_session_free(m_session); m_session = nullptr;
        close(m_sock); m_sock = -1; return false;
    }

    m_connected = true;
    std::cout << "[SSH] Connected to " << m_host << " as " << m_user << "\n";
    return true;
}

void RemoteSSHExecutor::disconnect()
{
    if (!m_connected) return;
    if (m_sftp)    { libssh2_sftp_shutdown(m_sftp); m_sftp = nullptr; }
    if (m_session) { libssh2_session_disconnect(m_session,"bye"); libssh2_session_free(m_session); m_session = nullptr; }
    if (m_sock != -1) { close(m_sock); m_sock = -1; }
    m_connected = false;
    std::cout << "[SSH] Disconnected from " << m_host << "\n";
}

std::pair<std::string,std::string>
RemoteSSHExecutor::executeCommand(const std::string& cmd)
{
    if (!m_connected) return {"","Not connected"};
    LIBSSH2_CHANNEL* ch = libssh2_channel_open_session(m_session);
    if (!ch) { logErr("open_session"); return {"","channel open failed"}; }
    if (libssh2_channel_exec(ch, cmd.c_str()) != 0) {
        logErr("exec"); libssh2_channel_free(ch); return {"","exec failed"};
    }
    std::string out, err;
    char buf[4096]; int n;
    while ((n = libssh2_channel_read(ch, buf, sizeof(buf))) > 0) out.append(buf, n);
    while ((n = libssh2_channel_read_stderr(ch, buf, sizeof(buf))) > 0) err.append(buf, n);
    libssh2_channel_close(ch);
    libssh2_channel_free(ch);
    return {out, err};
}

bool RemoteSSHExecutor::uploadFile(const std::string& local, const std::string& remote)
{
    if (!m_connected || !m_sftp) return false;
    std::ifstream lf(local, std::ios::binary);
    if (!lf) { std::cerr << "[SSH] Cannot open: " << local << "\n"; return false; }
    LIBSSH2_SFTP_HANDLE* h = libssh2_sftp_open(m_sftp, remote.c_str(),
        LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
        LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
    if (!h) { logErr("sftp_open_write"); return false; }
    char buf[4096];
    while (lf.read(buf, sizeof(buf)) || lf.gcount() > 0)
        if (libssh2_sftp_write(h, buf, static_cast<size_t>(lf.gcount())) < 0) {
            logErr("sftp_write"); libssh2_sftp_close(h); return false;
        }
    libssh2_sftp_close(h);
    return true;
}

bool RemoteSSHExecutor::downloadFile(const std::string& remote, const std::string& local)
{
    if (!m_connected || !m_sftp) return false;
    LIBSSH2_SFTP_HANDLE* h = libssh2_sftp_open(m_sftp, remote.c_str(), LIBSSH2_FXF_READ, 0);
    if (!h) { logErr("sftp_open_read"); return false; }
    std::ofstream lf(local, std::ios::binary);
    if (!lf) { libssh2_sftp_close(h); return false; }
    char buf[4096]; int n;
    while ((n = libssh2_sftp_read(h, buf, sizeof(buf))) > 0) lf.write(buf, n);
    libssh2_sftp_close(h);
    return true;
}

void RemoteSSHExecutor::logErr(const std::string& ctx) const
{
    if (m_session) {
        char* msg = nullptr;
        int code = libssh2_session_last_error(m_session, &msg, nullptr, 0);
        std::cerr << "[SSH] " << ctx << " failed (code=" << code << "): " << (msg?msg:"?") << "\n";
    } else {
        std::cerr << "[SSH] " << ctx << " failed (no session)\n";
    }
}

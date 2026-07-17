#include "UserEnumTechnique.h"
#include <iostream>

UserEnumTechnique::UserEnumTechnique(const std::string& name, const std::string& mitreId)
    : m_name(name), m_mitreId(mitreId) {}

bool UserEnumTechnique::execute(IExecutor& executor)
{
    if (!executor.isConnected()) { m_result = "Error: not connected"; return false; }

    std::cout << "\n[+] " << m_name << "  (" << m_mitreId << ")\n";

    std::string osHint = "linux";
    auto it = m_params.find("os");
    if (it != m_params.end()) osHint = it->second;

    std::string cmd;
    if (osHint == "windows") {
        cmd = "net user 2>nul";
    } else {
        cmd = "cat /etc/passwd | grep -v nologin | grep -v false | cut -d: -f1,3,6";
    }

    auto [out, err] = executor.executeCommand(cmd);
    m_result = out;
    if (!err.empty()) m_result += "\n[STDERR]\n" + err;

    m_detected = false;
    std::cout << "    STATUS: \033[1;32mClean\033[0m\n";
    if (!out.empty()) std::cout << "    OUTPUT:\n" << out;
    return true;
}

bool UserEnumTechnique::cleanup(IExecutor& executor) { (void)executor; return true; }

void UserEnumTechnique::setParameters(const std::map<std::string,std::string>& p)
{
    m_params = p;
}

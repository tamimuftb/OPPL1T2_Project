#include "ShellTechnique.h"
#include <iostream>
#include <algorithm>

const std::vector<std::string> ShellTechnique::s_keywords = {
    "mimikatz","meterpreter","nc -e","/dev/tcp","base64 -d",
    "powershell -enc","wget http","curl http","chmod +s","malicious","exploit"
};

ShellTechnique::ShellTechnique(const std::string& name, const std::string& mitreId, const std::string& cmd)
    : m_name(name), m_mitreId(mitreId), m_cmd(cmd) {}

bool ShellTechnique::execute(IExecutor& executor)
{
    if (!executor.isConnected()) { m_result = "Error: not connected"; return false; }

    std::cout << "\n[+] " << m_name << "  (" << m_mitreId << ")\n"
              << "    CMD: " << m_cmd << "\n";

    auto [out, err] = executor.executeCommand(m_cmd);
    m_result = out;
    if (!err.empty()) m_result += "\n[STDERR]\n" + err;

    std::string lower = m_cmd;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    m_detected = false;
    for (const auto& kw : s_keywords)
        if (lower.find(kw) != std::string::npos) { m_detected = true; break; }

    std::cout << "    STATUS: " << (m_detected ? "\033[1;31mDETECTED\033[0m" : "\033[1;32mClean\033[0m") << "\n";
    if (!out.empty()) std::cout << "    OUTPUT:\n" << out;
    return true;
}

bool ShellTechnique::cleanup(IExecutor& executor) { (void)executor; return true; }

void ShellTechnique::setParameters(const std::map<std::string,std::string>& p)
{
    m_params = p;
    auto it = p.find("command");
    if (it != p.end()) m_cmd = it->second;
}

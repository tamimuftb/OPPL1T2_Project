#include "ProcessEnumTechnique.h"
#include <iostream>

ProcessEnumTechnique::ProcessEnumTechnique(const std::string& name, const std::string& mitreId)
    : m_name(name), m_mitreId(mitreId) {}

bool ProcessEnumTechnique::execute(IExecutor& executor)
{
    if (!executor.isConnected()) { m_result = "Error: not connected"; return false; }

    std::cout << "\n[+] " << m_name << "  (" << m_mitreId << ")\n";

    // Check OS hint from params
    std::string osHint = "linux";
    auto it = m_params.find("os");
    if (it != m_params.end()) osHint = it->second;

    std::string cmd;
    if (osHint == "windows") {
        cmd = "tasklist /v 2>nul || Get-Process 2>nul";
    } else {
        cmd = "ps aux --no-headers 2>/dev/null | head -30";
    }

    auto [out, err] = executor.executeCommand(cmd);
    m_result  = out;
    if (!err.empty()) m_result += "\n[STDERR]\n" + err;

    m_detected = false;
    std::cout << "    STATUS: \033[1;32mClean\033[0m\n";
    if (!out.empty()) std::cout << "    OUTPUT:\n" << out;
    return true;
}

bool ProcessEnumTechnique::cleanup(IExecutor& executor) { (void)executor; return true; }

void ProcessEnumTechnique::setParameters(const std::map<std::string,std::string>& p)
{
    m_params = p;
}

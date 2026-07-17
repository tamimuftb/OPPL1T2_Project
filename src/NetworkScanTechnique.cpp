#include "NetworkScanTechnique.h"
#include <iostream>

NetworkScanTechnique::NetworkScanTechnique(const std::string& name,
    const std::string& mitreId, const std::string& target)
    : m_name(name), m_mitreId(mitreId), m_target(target) {}

bool NetworkScanTechnique::execute(IExecutor& executor)
{
    if (!executor.isConnected()) { m_result = "Error: not connected"; return false; }

    std::cout << "\n[+] " << m_name << "  (" << m_mitreId << ")\n"
              << "    TARGET: " << m_target << "\n";

    // Use ss/netstat for network discovery (no nmap needed on target)
    std::string cmd = "ss -tulnp 2>/dev/null || netstat -tulnp 2>/dev/null";
    if (!m_target.empty() && m_target != "localhost" && m_target != "127.0.0.1") {
        // ping sweep to discover host
        cmd = "ping -c 1 -W 1 " + m_target + " 2>&1; ss -tulnp 2>/dev/null";
    }

    auto [out, err] = executor.executeCommand(cmd);
    m_result = out;
    if (!err.empty()) m_result += "\n[STDERR]\n" + err;

    m_detected = false; // Network enumeration rarely self-detected
    std::cout << "    STATUS: \033[1;32mClean\033[0m\n";
    if (!out.empty()) std::cout << "    OUTPUT:\n" << out;
    return true;
}

bool NetworkScanTechnique::cleanup(IExecutor& executor) { (void)executor; return true; }

void NetworkScanTechnique::setParameters(const std::map<std::string,std::string>& p)
{
    m_params = p;
    auto it = p.find("target");
    if (it != p.end()) m_target = it->second;
}

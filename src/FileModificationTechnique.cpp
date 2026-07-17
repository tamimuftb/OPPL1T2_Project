#include "FileModificationTechnique.h"
#include <iostream>
#include <algorithm>

FileModificationTechnique::FileModificationTechnique(
    const std::string& name, const std::string& mitreId,
    const std::string& file, const std::string& content)
    : m_name(name), m_mitreId(mitreId), m_file(file), m_content(content) {}

bool FileModificationTechnique::execute(IExecutor& executor)
{
    if (!executor.isConnected()) { m_result = "Error: not connected"; return false; }

    std::cout << "\n[+] " << m_name << "  (" << m_mitreId << ")\n"
              << "    FILE: " << m_file << "\n"
              << "    DATA: " << m_content << "\n";

    std::string cmd = "echo \"" + m_content + "\" >> " + m_file;
    auto [out, err] = executor.executeCommand(cmd);
    m_result = out.empty() ? "(file written)" : out;
    if (!err.empty()) m_result += "\n[STDERR]\n" + err;

    std::string lc = m_content;
    std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
    m_detected = lc.find("malicious") != std::string::npos ||
                 lc.find("payload")   != std::string::npos ||
                 lc.find("exploit")   != std::string::npos;

    std::cout << "    STATUS: " << (m_detected ? "\033[1;31mDETECTED\033[0m" : "\033[1;32mClean\033[0m") << "\n";
    return true;
}

bool FileModificationTechnique::cleanup(IExecutor& executor)
{
    executor.executeCommand("rm -f " + m_file);
    return true;
}

void FileModificationTechnique::setParameters(const std::map<std::string,std::string>& p)
{
    m_params = p;
    auto fi = p.find("file"), ci = p.find("content");
    if (fi != p.end()) m_file    = fi->second;
    if (ci != p.end()) m_content = ci->second;
}

#include "PlaybookEngine.h"
#include "TechniqueFactory.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

PlaybookEngine::PlaybookEngine(std::unique_ptr<IExecutor> executor)
    : m_executor(std::move(executor)) {}

bool PlaybookEngine::loadPlaybook(const std::string& path)
{
    YAML::Node root;
    try { root = YAML::LoadFile(path); }
    catch (const YAML::Exception& e) {
        std::cerr << "[Playbook] Parse error: " << e.what() << "\n";
        return false;
    }

    m_playbookName = root["playbook_name"] ? root["playbook_name"].as<std::string>() : "(unnamed)";
    m_targetOS     = root["target_os"]     ? root["target_os"].as<std::string>()     : "linux";

    if (!root["techniques"]) {
        std::cerr << "[Playbook] No 'techniques' section\n";
        return false;
    }

    std::cout << "\n[Playbook] " << m_playbookName << "  (target_os: " << m_targetOS << ")\n";

    for (const auto& node : root["techniques"]) {
        std::string name    = node["name"]     ? node["name"].as<std::string>()     : "Unknown";
        std::string mitreId = node["mitre_id"] ? node["mitre_id"].as<std::string>() : "T0000";
        std::string type    = node["type"]     ? node["type"].as<std::string>()     : "";

        if (type.empty()) {
            std::cerr << "  [-] '" << name << "' has no type — skipped\n";
            continue;
        }

        std::map<std::string,std::string> params;
        if (node["parameters"])
            for (const auto& kv : node["parameters"])
                params[kv.first.as<std::string>()] = kv.second.as<std::string>();

        params["os"] = m_targetOS;

        auto tech = TechniqueFactory::createTechnique(type, name, mitreId, params);
        if (tech) {
            m_techniques.push_back(std::move(tech));
            std::cout << "  [+] " << name << "  (" << mitreId << ")\n";
        } else {
            std::cerr << "  [-] Failed: " << name << "\n";
        }
    }

    std::cout << "[Playbook] " << m_techniques.size() << " technique(s) ready\n";
    return !m_techniques.empty();
}

void PlaybookEngine::executeCampaign()
{
    if (!m_executor->isConnected()) {
        std::cout << "[Engine] Connecting...\n";
        if (!m_executor->connect()) {
            std::cerr << "[Engine] Connection failed. Abort.\n";
            return;
        }
    }

    std::cout << "\n\033[1;34m╔══════════════════════════════════╗\033[0m\n"
              << "\033[1;34m║   SENTINEL-RED  CAMPAIGN START   ║\033[0m\n"
              << "\033[1;34m╚══════════════════════════════════╝\033[0m\n";

    int ok = 0, fail = 0, detected = 0;
    for (const auto& tech : m_techniques) {
        std::cout << "\033[90m──────────────────────────────────\033[0m\n";
        bool r = tech->execute(*m_executor);
        r ? ++ok : ++fail;
        if (tech->isDetected()) ++detected;
    }

    std::cout << "\n\033[1;34m╔══════════════════════════════════╗\033[0m\n"
              << "\033[1;34m║         CAMPAIGN COMPLETE        ║\033[0m\n"
              << "\033[1;34m╠══════════════════════════════════╣\033[0m\n"
              << "\033[1;34m║\033[0m  Techniques : " << (ok+fail) << "\n"
              << "\033[1;34m║\033[0m  Passed     : \033[1;32m" << ok << "\033[0m\n"
              << "\033[1;34m║\033[0m  Failed     : \033[1;31m" << fail << "\033[0m\n"
              << "\033[1;34m║\033[0m  Detected   : \033[1;33m" << detected << "\033[0m\n"
              << "\033[1;34m╚══════════════════════════════════╝\033[0m\n\n";

    m_executor->disconnect();

    if (m_reporter) m_reporter->generateReport(m_techniques);
}

void PlaybookEngine::setReporter(std::unique_ptr<IReporter> r) { m_reporter = std::move(r); }

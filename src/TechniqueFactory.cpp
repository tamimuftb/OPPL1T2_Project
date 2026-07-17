#include "TechniqueFactory.h"
#include "ShellTechnique.h"
#include "FileModificationTechnique.h"
#include "NetworkScanTechnique.h"
#include "ProcessEnumTechnique.h"
#include "UserEnumTechnique.h"
#include <iostream>

std::map<std::string,TechniqueFactory::CreateFn>& TechniqueFactory::registry()
{
    static std::map<std::string,CreateFn> reg;
    return reg;
}

void TechniqueFactory::registerTechnique(const std::string& type, CreateFn fn)
{
    registry()[type] = std::move(fn);
}

std::unique_ptr<ITechnique> TechniqueFactory::createTechnique(
    const std::string& type, const std::string& name,
    const std::string& mitreId, const std::map<std::string,std::string>& params)
{
    auto it = registry().find(type);
    if (it == registry().end()) {
        std::cerr << "[Factory] Unknown type: '" << type << "'\n";
        return nullptr;
    }
    return it->second(name, mitreId, params);
}

namespace {
struct Registrar {
    Registrar() {
        // shell_command
        TechniqueFactory::registerTechnique("shell_command",
            [](const std::string& n, const std::string& m,
               const std::map<std::string,std::string>& p) -> std::unique_ptr<ITechnique> {
                auto it = p.find("command");
                if (it == p.end()) {
                    std::cerr << "[Factory] shell_command missing 'command'\n";
                    return std::unique_ptr<ITechnique>(nullptr);
                }
                return std::make_unique<ShellTechnique>(n, m, it->second);
            });

        // file_modification
        TechniqueFactory::registerTechnique("file_modification",
            [](const std::string& n, const std::string& m,
               const std::map<std::string,std::string>& p) -> std::unique_ptr<ITechnique> {
                auto fi = p.find("file"), ci = p.find("content");
                if (fi == p.end() || ci == p.end()) {
                    std::cerr << "[Factory] file_modification missing 'file' or 'content'\n";
                    return std::unique_ptr<ITechnique>(nullptr);
                }
                return std::make_unique<FileModificationTechnique>(n, m, fi->second, ci->second);
            });

        // network_scan
        TechniqueFactory::registerTechnique("network_scan",
            [](const std::string& n, const std::string& m,
               const std::map<std::string,std::string>& p) -> std::unique_ptr<ITechnique> {
                std::string target = "localhost";
                auto it = p.find("target");
                if (it != p.end()) target = it->second;
                auto tech = std::make_unique<NetworkScanTechnique>(n, m, target);
                tech->setParameters(p);
                return tech;
            });

        // process_enum
        TechniqueFactory::registerTechnique("process_enum",
            [](const std::string& n, const std::string& m,
               const std::map<std::string,std::string>& p) -> std::unique_ptr<ITechnique> {
                auto tech = std::make_unique<ProcessEnumTechnique>(n, m);
                tech->setParameters(p);
                return tech;
            });

        // user_enum
        TechniqueFactory::registerTechnique("user_enum",
            [](const std::string& n, const std::string& m,
               const std::map<std::string,std::string>& p) -> std::unique_ptr<ITechnique> {
                auto tech = std::make_unique<UserEnumTechnique>(n, m);
                tech->setParameters(p);
                return tech;
            });
    }
};
static Registrar s_reg;
} // namespace

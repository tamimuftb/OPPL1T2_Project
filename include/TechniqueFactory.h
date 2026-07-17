#pragma once
#include "ITechnique.h"
#include <string>
#include <map>
#include <memory>
#include <functional>

class TechniqueFactory {
public:
    using CreateFn = std::function<
        std::unique_ptr<ITechnique>(
            const std::string&,
            const std::string&,
            const std::map<std::string,std::string>&)>;

    static void registerTechnique(const std::string& type, CreateFn fn);
    static std::unique_ptr<ITechnique> createTechnique(
        const std::string& type,
        const std::string& name,
        const std::string& mitreId,
        const std::map<std::string,std::string>& params);
private:
    static std::map<std::string,CreateFn>& registry();
};

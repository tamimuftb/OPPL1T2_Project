#pragma once
#include "ITechnique.h"
#include "IExecutor.h"
#include "IReporter.h"
#include <string>
#include <vector>
#include <memory>

class PlaybookEngine {
public:
    explicit PlaybookEngine(std::unique_ptr<IExecutor> executor);
    ~PlaybookEngine() = default;
    bool loadPlaybook(const std::string& path);
    void executeCampaign();
    void setReporter(std::unique_ptr<IReporter> reporter);
private:
    std::unique_ptr<IExecutor>               m_executor;
    std::unique_ptr<IReporter>               m_reporter;
    std::vector<std::unique_ptr<ITechnique>> m_techniques;
    std::string                              m_playbookName;
    std::string                              m_targetOS;
};

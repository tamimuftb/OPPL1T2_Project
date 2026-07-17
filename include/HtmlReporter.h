#pragma once
#include "IReporter.h"
#include <string>
#include <fstream>

class HtmlReporter : public IReporter {
public:
    explicit HtmlReporter(const std::string& path, const std::string& playbookName = "",
                          const std::string& targetOS = "", const std::string& targetHost = "");
    ~HtmlReporter() = default;
    void generateReport(const std::vector<std::unique_ptr<ITechnique>>& techniques) override;
private:
    std::string m_path, m_playbookName, m_targetOS, m_targetHost;
    void writeHeader(std::ofstream& f) const;
    void writeFooter(std::ofstream& f) const;
    void writeTechnique(std::ofstream& f, const ITechnique& t) const;
    static std::string esc(const std::string& s);
};

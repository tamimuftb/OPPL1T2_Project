#pragma once
#include "ITechnique.h"
#include <string>
#include <map>

class ProcessEnumTechnique : public ITechnique {
public:
    ProcessEnumTechnique(const std::string& name, const std::string& mitreId);
    ~ProcessEnumTechnique() = default;
    bool execute(IExecutor& executor) override;
    bool cleanup(IExecutor& executor) override;
    std::string getName()    const override { return m_name; }
    std::string getMitreId() const override { return m_mitreId; }
    void setParameters(const std::map<std::string,std::string>& p) override;
    std::map<std::string,std::string> getParameters() const override { return m_params; }
    std::string getResult()  const override { return m_result; }
    void setResult(const std::string& r) override { m_result = r; }
    bool isDetected()        const override { return m_detected; }
    void setDetected(bool v) override { m_detected = v; }
private:
    std::string m_name, m_mitreId, m_result;
    std::map<std::string,std::string> m_params;
    bool m_detected = false;
};

#pragma once
#include <string>
#include <map>
#include "IExecutor.h"

class ITechnique {
public:
    virtual ~ITechnique() = default;
    virtual bool execute(IExecutor& executor) = 0;
    virtual bool cleanup(IExecutor& executor) = 0;
    virtual std::string getName()    const = 0;
    virtual std::string getMitreId() const = 0;
    virtual void setParameters(const std::map<std::string,std::string>& p) = 0;
    virtual std::map<std::string,std::string> getParameters() const = 0;
    virtual std::string getResult()  const = 0;
    virtual void setResult(const std::string& r) = 0;
    virtual bool isDetected()        const = 0;
    virtual void setDetected(bool v) = 0;
};

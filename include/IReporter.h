#pragma once
#include <vector>
#include <memory>
#include "ITechnique.h"

class IReporter {
public:
    virtual ~IReporter() = default;
    virtual void generateReport(const std::vector<std::unique_ptr<ITechnique>>& techniques) = 0;
};

#pragma once

#include "AiDirector.h"

class RuleBasedDirector : public AiDirector {
public:
    std::string buildHint(const GameSnapshot& snapshot) override;
};

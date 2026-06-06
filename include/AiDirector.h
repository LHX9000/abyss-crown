#pragma once

#include <string>

#include "Types.h"

class AiDirector {
public:
    virtual ~AiDirector() = default;
    virtual std::string buildHint(const GameSnapshot& snapshot) = 0;
};

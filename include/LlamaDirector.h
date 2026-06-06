#pragma once

#include <memory>
#include <string>

#include "AiDirector.h"

struct LlamaConfig {
    bool enabled = false;
    std::string executablePath;
    std::string modelPath;
};

class LlamaDirector : public AiDirector {
public:
    explicit LlamaDirector(LlamaConfig config, std::unique_ptr<AiDirector> fallback);
    std::string buildHint(const GameSnapshot& snapshot) override;

private:
    std::string callLlama(const std::string& prompt) const;
    std::string buildPrompt(const GameSnapshot& snapshot) const;

    LlamaConfig config_;
    std::unique_ptr<AiDirector> fallback_;
};

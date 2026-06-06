#include "LlamaDirector.h"

#include <cstdio>
#include <sstream>

namespace {
std::string shellEscape(const std::string& value) {
#ifdef _WIN32
    std::string escaped = "\"";
    for (char ch : value) {
        if (ch == '"') {
            escaped += "\\\"";
        } else {
            escaped += ch;
        }
    }
    escaped += "\"";
    return escaped;
#else
    std::string escaped = "'";
    for (char ch : value) {
        if (ch == '\'') {
            escaped += "'\\''";
        } else {
            escaped += ch;
        }
    }
    escaped += "'";
    return escaped;
#endif
}

bool hasRequiredConfig(const LlamaConfig& config) {
    return config.enabled && !config.executablePath.empty() && !config.modelPath.empty();
}
}

LlamaDirector::LlamaDirector(LlamaConfig config, std::unique_ptr<AiDirector> fallback)
    : config_(std::move(config)), fallback_(std::move(fallback)) {}

std::string LlamaDirector::buildHint(const GameSnapshot& snapshot) {
    if (!hasRequiredConfig(config_)) {
        return fallback_->buildHint(snapshot);
    }

    const std::string generated = callLlama(buildPrompt(snapshot));
    if (generated.empty()) {
        return fallback_->buildHint(snapshot);
    }
    return generated;
}

std::string LlamaDirector::buildPrompt(const GameSnapshot& snapshot) const {
    std::ostringstream prompt;
    prompt << "请使用中文写一句简短、有氛围感的地牢游戏提示。"
           << "玩家当前有 " << snapshot.health << " 点生命值，已收集 "
           << snapshot.shardCount << "/" << snapshot.requiredShards
           << " 个记忆碎片，坐标为 (" << snapshot.playerPosition.x << ", "
           << snapshot.playerPosition.y << ")。提示不超过 25 个汉字。";
    return prompt.str();
}

std::string LlamaDirector::callLlama(const std::string& prompt) const {
    const std::string command = shellEscape(config_.executablePath)
        + " -m " + shellEscape(config_.modelPath)
        + " -p " + shellEscape(prompt)
        + " -n 40 --no-display-prompt "
#ifdef _WIN32
        + "2>NUL";
#else
        + "2>/dev/null";
#endif

#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (pipe == nullptr) {
        return "";
    }

    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
        if (output.size() > 240) {
            break;
        }
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    while (!output.empty() && (output.back() == '\n' || output.back() == '\r' || output.back() == ' ')) {
        output.pop_back();
    }
    return output;
}

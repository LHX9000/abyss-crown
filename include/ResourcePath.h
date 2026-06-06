#pragma once

#include <filesystem>
#include <string>

namespace ResourcePath {

void initialize();
std::filesystem::path resolve(const std::filesystem::path& relativePath);
std::string resolveString(const std::filesystem::path& relativePath);

}

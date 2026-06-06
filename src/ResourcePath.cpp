#include "ResourcePath.h"

#include <SDL.h>

#include <vector>

namespace {
std::vector<std::filesystem::path> searchRoots;
}

void ResourcePath::initialize() {
    searchRoots.clear();

    char* basePath = SDL_GetBasePath();
    if (basePath != nullptr) {
        searchRoots.emplace_back(basePath);
        SDL_free(basePath);
    }

    searchRoots.emplace_back(std::filesystem::current_path());
    searchRoots.emplace_back(std::filesystem::current_path().parent_path());
    searchRoots.emplace_back(std::filesystem::current_path().parent_path().parent_path());
}

std::filesystem::path ResourcePath::resolve(const std::filesystem::path& relativePath) {
    for (const auto& root : searchRoots) {
        const auto candidate = root / relativePath;
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }
    return relativePath;
}

std::string ResourcePath::resolveString(const std::filesystem::path& relativePath) {
    return resolve(relativePath).u8string();
}

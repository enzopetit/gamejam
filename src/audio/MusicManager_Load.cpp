#include "audio/MusicManager.hpp"
#include "game/GameConfig.hpp"

#include <fstream>
#include <regex>

namespace {
std::string extractStringValue(const std::string& content, const std::string& key) {
    std::regex re("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"");
    std::smatch match;
    if (std::regex_search(content, match, re))
        return match[1].str();
    return {};
}

std::string extractObjectBlock(const std::string& content, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t keyPos = content.find(needle);
    if (keyPos == std::string::npos) return {};
    size_t bracePos = content.find('{', keyPos);
    if (bracePos == std::string::npos) return {};
    int depth = 0;
    for (size_t i = bracePos; i < content.size(); ++i) {
        if (content[i] == '{') depth++;
        else if (content[i] == '}') {
            depth--;
            if (depth == 0)
                return content.substr(bracePos, i - bracePos + 1);
        }
    }
    return {};
}
}

bool MusicManager::loadFromJson(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return false;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    beginTrack = extractStringValue(content, "begin");
    perMinute.clear();
    std::string block = extractObjectBlock(content, "per_minute");
    if (!block.empty()) {
        std::regex re("\\\"(\\d+)\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"");
        auto it = std::sregex_iterator(block.begin(), block.end(), re);
        auto end = std::sregex_iterator();
        for (; it != end; ++it) {
            int minute = std::stoi((*it)[1].str());
            perMinute[minute] = (*it)[2].str();
        }
    }
    ready = !perMinute.empty();
    return ready;
}


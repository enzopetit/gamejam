#include "audio/MusicManager.hpp"

#include "game/GameConfig.hpp"

#include <algorithm>
#include <cmath>
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

void MusicManager::reset(float timeLeft) {
    currentMinute = 0;
    currentTrack.clear();
    beginPlaying = false;

    if (!ready)
        return;

    if (!beginTrack.empty()) {
        beginPlaying = playTrack(beginTrack, false);
        if (beginPlaying)
            return;
    }

    int target = minuteBucket(timeLeft);
    auto it = perMinute.find(target);
    if (it != perMinute.end()) {
        playTrack(it->second, true);
        currentMinute = target;
    }
}

void MusicManager::update(float timeLeft) {
    if (!ready)
        return;

    if (beginPlaying) {
        if (music.getStatus() == sf::SoundSource::Status::Playing)
            return;
        beginPlaying = false;
    }

    int target = minuteBucket(timeLeft);
    if (currentMinute == 0) {
        auto it = perMinute.find(target);
        if (it != perMinute.end()) {
            playTrack(it->second, true);
            currentMinute = target;
        }
        return;
    }

    if (target < currentMinute) {
        auto it = perMinute.find(target);
        if (it != perMinute.end()) {
            playTrack(it->second, true);
            currentMinute = target;
        }
    }
}

bool MusicManager::playTrack(const std::string& path, bool loop) {
    if (path.empty())
        return false;
    if (path == currentTrack && music.getStatus() == sf::SoundSource::Status::Playing)
        return true;
    if (!music.openFromFile(path))
        return false;
    music.setLooping(loop);
    music.play();
    currentTrack = path;
    return true;
}

int MusicManager::minuteBucket(float timeLeft) const {
    float clamped = std::clamp(timeLeft, 0.0f, TOTAL_TIME);
    int minute = static_cast<int>(std::ceil(clamped / 60.0f));
    if (minute < 1) minute = 1;
    int maxMin = static_cast<int>(std::ceil(TOTAL_TIME / 60.0f));
    if (minute > maxMin) minute = maxMin;
    return minute;
}

#include "audio/MusicManager.hpp"

#include "game/GameConfig.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <regex>
#include <utility>

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
    stop();

    currentMinute = 0;
    currentTrack.clear();
    pendingTrack.clear();
    pendingMinute = 0;
    beginPlaying = false;
    fading = false;
    fadeProgress = 0.0f;

    if (!ready)
        return;

    if (!beginTrack.empty()) {
        beginPlaying = startTrack(current, beginTrack, false, volume);
        if (beginPlaying)
            currentTrack = beginTrack;
        if (beginPlaying)
            return;
    }

    int target = minuteBucket(timeLeft);
    auto it = perMinute.find(target);
    if (it != perMinute.end()) {
        startTrack(current, it->second, true, volume);
        currentMinute = target;
        currentTrack = it->second;
    }
}

void MusicManager::update(float timeLeft, float dt) {
    if (!ready)
        return;

    if (beginPlaying) {
        if (current->getStatus() == sf::SoundSource::Status::Playing)
            return;
        beginPlaying = false;
    }

    int target = minuteBucket(timeLeft);
    if (currentMinute == 0) {
        auto it = perMinute.find(target);
        if (it != perMinute.end()) {
            startTrack(current, it->second, true, volume);
            currentMinute = target;
            currentTrack = it->second;
        }
        return;
    }

    if (fading) {
        fadeProgress += dt;
        applyVolumes();
        if (fadeProgress >= fadeDuration) {
            current->stop();
            std::swap(current, pending);
            currentTrack = pendingTrack;
            currentMinute = pendingMinute;
            pendingTrack.clear();
            pendingMinute = 0;
            fading = false;
            fadeProgress = 0.0f;
            applyVolumes();
        }
        return;
    }

    if (target < currentMinute) {
        auto it = perMinute.find(target);
        if (it != perMinute.end() && it->second != currentTrack) {
            if (startTrack(pending, it->second, true, 0.0f)) {
                pendingTrack = it->second;
                pendingMinute = target;
                fading = true;
                fadeProgress = 0.0f;
                applyVolumes();
            }
        }
    }
}

void MusicManager::setVolume(float value) {
    volume = std::clamp(value, 0.0f, 100.0f);
    applyVolumes();
}

void MusicManager::stop() {
    musicA.stop();
    musicB.stop();
    currentTrack.clear();
    pendingTrack.clear();
    currentMinute = 0;
    pendingMinute = 0;
    beginPlaying = false;
    fading = false;
    fadeProgress = 0.0f;
}

bool MusicManager::startTrack(sf::Music* track, const std::string& path, bool loop, float vol) {
    if (path.empty())
        return false;
    if (!track->openFromFile(path))
        return false;
    track->setLooping(loop);
    track->setVolume(vol);
    track->play();
    return true;
}

void MusicManager::applyVolumes() {
    if (fading) {
        float t = std::clamp(fadeProgress / fadeDuration, 0.0f, 1.0f);
        current->setVolume(volume * (1.0f - t));
        pending->setVolume(volume * t);
        return;
    }
    current->setVolume(volume);
}

int MusicManager::minuteBucket(float timeLeft) const {
    float clamped = std::clamp(timeLeft, 0.0f, TOTAL_TIME);
    int minute = static_cast<int>(std::ceil(clamped / 60.0f));
    if (minute < 1) minute = 1;
    int maxMin = static_cast<int>(std::ceil(TOTAL_TIME / 60.0f));
    if (minute > maxMin) minute = maxMin;
    return minute;
}

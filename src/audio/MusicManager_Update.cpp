#include "audio/MusicManager.hpp"
#include "game/GameConfig.hpp"

#include <algorithm>
#include <cmath>

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
        if (beginPlaying) currentTrack = beginTrack;
        if (beginPlaying) return;
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
    if (!ready) return;
    if (beginPlaying) {
        if (current->getStatus() == sf::SoundSource::Status::Playing) return;
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

int MusicManager::minuteBucket(float timeLeft) const {
    float clamped = std::clamp(timeLeft, 0.0f, TOTAL_TIME);
    int minute = static_cast<int>(std::ceil(clamped / 60.0f));
    if (minute < 1) minute = 1;
    int maxMin = static_cast<int>(std::ceil(TOTAL_TIME / 60.0f));
    if (minute > maxMin) minute = maxMin;
    return minute;
}


#include "audio/MusicManager.hpp"

#include <algorithm>

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
    if (path.empty()) return false;
    if (!track->openFromFile(path)) return false;
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


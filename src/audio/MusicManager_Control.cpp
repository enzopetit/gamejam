#include "audio/MusicManager.hpp"
#include "game/GameConfig.hpp"
#include <algorithm>
#include <cmath>

void MusicManager::setVolume(float value) { volume = std::clamp(value, 0.0f, 100.0f); applyVolumes(); }

void MusicManager::stop() { musicA.stop(); musicB.stop(); currentTrack.clear(); pendingTrack.clear(); currentMinute=0; pendingMinute=0; beginPlaying=false; fading=false; fadeProgress=0.0f; }

bool MusicManager::startTrack(sf::Music* track, const std::string& path, bool loop, float vol) {
    if (path.empty()) return false; if (!track->openFromFile(path)) return false; track->setLooping(loop); track->setVolume(vol); track->play(); return true;
}

int MusicManager::minuteBucket(float timeLeft) const {
    float clamped = std::clamp(timeLeft, 0.0f, TOTAL_TIME); int minute = static_cast<int>(std::ceil(clamped/60.0f)); if (minute<1) minute=1; int maxMin = static_cast<int>(std::ceil(TOTAL_TIME/60.0f)); if (minute>maxMin) minute=maxMin; return minute;
}


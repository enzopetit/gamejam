#pragma once

#include <SFML/Audio/Music.hpp>

#include <string>
#include <unordered_map>

class MusicManager {
public:
    bool loadFromJson(const std::string& path);
    void reset(float timeLeft);
    void update(float timeLeft, float dt);
    void setVolume(float value);
    void stop();

private:
    bool startTrack(sf::Music* track, const std::string& path, bool loop, float vol);
    void applyVolumes();
    int minuteBucket(float timeLeft) const;

    sf::Music musicA;
    sf::Music musicB;
    sf::Music* current = &musicA;
    sf::Music* pending = &musicB;
    std::unordered_map<int, std::string> perMinute;
    std::string beginTrack;
    std::string currentTrack;
    std::string pendingTrack;
    int currentMinute = 0;
    int pendingMinute = 0;
    bool beginPlaying = false;
    bool ready = false;
    bool fading = false;
    float fadeProgress = 0.0f;
    float fadeDuration = 1.0f;
    float volume = 100.0f;
};

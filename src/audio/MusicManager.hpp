#pragma once

#include <SFML/Audio/Music.hpp>

#include <string>
#include <unordered_map>

class MusicManager {
public:
    bool loadFromJson(const std::string& path);
    void reset(float timeLeft);
    void update(float timeLeft);

private:
    bool playTrack(const std::string& path, bool loop);
    int minuteBucket(float timeLeft) const;

    sf::Music music;
    std::unordered_map<int, std::string> perMinute;
    std::string beginTrack;
    std::string currentTrack;
    int currentMinute = 0;
    bool beginPlaying = false;
    bool ready = false;
};

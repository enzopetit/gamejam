#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class SoundManager {
public:
    void loadSound(const std::string& name, const std::string& path);
    void play(const std::string& name, float pitch = 1.0f, float volume = 100.0f);

private:
    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::vector<sf::Sound> pool;
};

#include "audio/SoundManager.hpp"

void SoundManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer buf;
    if (buf.loadFromFile(path))
        buffers[name] = std::move(buf);
}

void SoundManager::play(const std::string& name, float pitch, float volume) {
    auto it = buffers.find(name);
    if (it == buffers.end()) return;

    for (auto& s : pool) {
        if (s.getStatus() == sf::SoundSource::Status::Stopped) {
            s.setBuffer(it->second);
            s.setPitch(pitch);
            s.setVolume(volume);
            s.play();
            return;
        }
    }
    if (pool.size() < 32) {
        pool.emplace_back(it->second);
        pool.back().setPitch(pitch);
        pool.back().setVolume(volume);
        pool.back().play();
    }
}

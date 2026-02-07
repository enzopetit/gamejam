#include "game/HighscoreManager.hpp"

#include <algorithm>
#include <fstream>

void HighscoreManager::load(const std::string& path) {
    scores.clear();
    std::ifstream f(path, std::ios::binary);
    if (!f) return;
    int count = 0;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count && i < maxEntries; ++i) {
        int s = 0;
        f.read(reinterpret_cast<char*>(&s), sizeof(s));
        scores.push_back(s);
    }
    std::sort(scores.begin(), scores.end(), std::greater<int>());
}

void HighscoreManager::save(const std::string& path) const {
    std::ofstream f(path, std::ios::binary);
    if (!f) return;
    int count = static_cast<int>(scores.size());
    f.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int s : scores)
        f.write(reinterpret_cast<const char*>(&s), sizeof(s));
}

bool HighscoreManager::isHighscore(int score) const {
    if (score <= 0) return false;
    if (static_cast<int>(scores.size()) < maxEntries) return true;
    return score > scores.back();
}

void HighscoreManager::addScore(int score) {
    scores.push_back(score);
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if (static_cast<int>(scores.size()) > maxEntries)
        scores.resize(maxEntries);
}

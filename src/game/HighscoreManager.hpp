#pragma once

#include <string>
#include <vector>

class HighscoreManager {
public:
    void load(const std::string& path);
    void save(const std::string& path) const;
    bool isHighscore(int score) const;
    void addScore(int score);
    const std::vector<int>& getScores() const { return scores; }

private:
    std::vector<int> scores;
    int maxEntries = 10;
};

#include "game/HudRenderer.hpp"

void hudDrawTimeBar(sf::RenderWindow& window, const sf::Font& font, float timeLeft);
void hudDrawBuffsObjectives(sf::RenderWindow& window, const sf::Font& font, float timeLeft, float speedBoost, float rapidFire, int score, int combo, const LevelManager& level);

void drawHud(sf::RenderWindow& window, const sf::Font& font, float timeLeft, int score, int comboCount, float speedBoostTimer, float rapidFireTimer, const LevelManager& level) {
    hudDrawTimeBar(window, font, timeLeft);
    hudDrawBuffsObjectives(window, font, timeLeft, speedBoostTimer, rapidFireTimer, score, comboCount, level);
}


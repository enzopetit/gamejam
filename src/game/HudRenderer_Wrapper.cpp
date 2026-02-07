#include "game/HudRenderer.hpp"

void hudDrawTimeBar(sf::RenderWindow& window, const sf::Font& font, float timeLeft);
void hudDrawBuffsObjectives(sf::RenderWindow& window, const sf::Font& font, float timeLeft, float speedBoost, float rapidFire, float pierce, float laserCharge, float laserTimer, int score, int combo, const LevelManager& level);

void drawHud(sf::RenderWindow& window, const sf::Font& font, float timeLeft, int score, int comboCount, float speedBoostTimer, float rapidFireTimer, float pierceTimer, float laserCharge, float laserTimer, const LevelManager& level) {
    hudDrawTimeBar(window, font, timeLeft);
    hudDrawBuffsObjectives(window, font, timeLeft, speedBoostTimer, rapidFireTimer, pierceTimer, laserCharge, laserTimer, score, comboCount, level);
}

#pragma once

#include "game/LevelManager.hpp"
#include <SFML/Graphics.hpp>

void drawHud(
    sf::RenderWindow& window,
    const sf::Font& font,
    float timeLeft,
    int score,
    int comboCount,
    float speedBoostTimer,
    float rapidFireTimer,
    const LevelManager& level
);

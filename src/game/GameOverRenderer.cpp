#include "game/GameOverRenderer.hpp"

#include "game/GameConfig.hpp"

void drawGameOverScreen(sf::RenderWindow& window, const sf::Font* font, int score) {
    window.clear(sf::Color(30, 10, 10));
    if (!font) return;

    sf::Text gameOverText(*font, "GAME OVER", 60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({WIN_W / 2.0f - 180.0f, WIN_H / 2.0f - 80.0f});
    window.draw(gameOverText);

    sf::Text scoreText(*font, "Score: " + std::to_string(score), 30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({WIN_W / 2.0f - 80.0f, WIN_H / 2.0f});
    window.draw(scoreText);

    sf::Text restartText(*font, "Appuie sur R pour recommencer", 20);
    restartText.setFillColor(sf::Color(200, 200, 200));
    restartText.setPosition({WIN_W / 2.0f - 150.0f, WIN_H / 2.0f + 50.0f});
    window.draw(restartText);
}


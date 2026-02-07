#include "game/HudRenderer.hpp"
#include "game/GameConfig.hpp"
#include <cmath>

void hudDrawTimeBar(sf::RenderWindow& window, const sf::Font& font, float timeLeft) {
    int minutes = static_cast<int>(timeLeft) / 60; int seconds = static_cast<int>(timeLeft) % 60; int centis = static_cast<int>((timeLeft - std::floor(timeLeft)) * 100);
    std::string timeStr = std::to_string(minutes) + ":" + (seconds<10?"0":"") + std::to_string(seconds) + "." + (centis<10?"0":"") + std::to_string(centis);
    sf::Text timerText(font, timeStr, 36); timerText.setFillColor(timeLeft<30.0f ? sf::Color::Red : sf::Color::White); timerText.setPosition({WIN_W/2.0f - 70.0f, 10.0f}); window.draw(timerText);
    float barWidth=300.0f, barHeight=8.0f, barX=WIN_W/2.0f - barWidth/2.0f, barY=55.0f;
    sf::RectangleShape barBg({barWidth, barHeight}); barBg.setFillColor(sf::Color(60,60,60)); barBg.setPosition({barX, barY}); window.draw(barBg);
    float fillW = barWidth * (timeLeft / TOTAL_TIME); sf::RectangleShape barFill({fillW, barHeight});
    if (timeLeft < 30.0f) barFill.setFillColor(sf::Color::Red); else if (timeLeft < 60.0f) barFill.setFillColor(sf::Color(255,165,0)); else barFill.setFillColor(sf::Color::Green);
    barFill.setPosition({barX, barY}); window.draw(barFill);
}

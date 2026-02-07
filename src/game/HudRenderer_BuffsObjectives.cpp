#include "game/HudRenderer.hpp"
#include "game/GameConfig.hpp"
#include "actors/Actor.hpp"

#include <algorithm>
#include <cmath>

namespace {
std::string fmtTenths(float v) {
    if (v < 0.0f) v = 0.0f;
    int tt = static_cast<int>(std::ceil(v * 10.0f));
    int w = tt / 10;
    int t = tt % 10;
    return std::to_string(w) + "." + std::to_string(t);
}

sf::Color buffFill(BonusType t) {
    if (t == BonusType::Speed) return sf::Color(80, 200, 255);
    if (t == BonusType::RapidFire) return sf::Color(255, 200, 80);
    if (t == BonusType::Pierce) return sf::Color(80, 220, 120);
    return sf::Color(255, 80, 80);
}

sf::Color buffOutline(BonusType t) {
    if (t == BonusType::Speed) return sf::Color(120, 255, 255);
    if (t == BonusType::RapidFire) return sf::Color(255, 255, 120);
    if (t == BonusType::Pierce) return sf::Color(140, 255, 170);
    return sf::Color(255, 200, 50);
}

std::string buffName(BonusType t) {
    if (t == BonusType::Speed) return "VITESSE";
    if (t == BonusType::RapidFire) return "RAPIDE";
    if (t == BonusType::Pierce) return "PERCE";
    return "TEMPS";
}

void centerText(sf::Text& text, sf::Vector2f pos) {
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin({b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
    text.setPosition(pos);
}

void drawBuff(sf::RenderWindow& window, const sf::Font& font, BonusType type, float rem, float iconY, float iconSize, float iconGap, float& iconX) {
    if (rem <= 0.0f) return;

    std::string name = buffName(type);
    sf::Text label(font, name, 12);
    label.setFillColor(sf::Color(230, 230, 230));
    sf::FloatRect lb = label.getLocalBounds();
    float slotW = std::max(iconSize, lb.size.x) + 6.0f;
    float cx = iconX + slotW / 2.0f;

    sf::RectangleShape icon({iconSize, iconSize});
    icon.setFillColor(buffFill(type));
    icon.setOutlineColor(buffOutline(type));
    icon.setOutlineThickness(2.0f);
    icon.setPosition({cx - iconSize / 2.0f, iconY});
    window.draw(icon);

    sf::Text value(font, fmtTenths(rem), 14);
    value.setFillColor(sf::Color::Black);
    centerText(value, {cx, iconY + iconSize / 2.0f});
    window.draw(value);

    centerText(label, {cx, iconY + iconSize + 10.0f});
    window.draw(label);
    iconX += slotW + iconGap;
}

void drawObjectives(sf::RenderWindow& window, const sf::Font& font, float objectiveY, const LevelManager& level) {
    if (level.isIntermission()) {
        sf::Text inter(font, "VAGUE TERMINEE", 18);
        inter.setFillColor(sf::Color(230, 230, 230));
        centerText(inter, {WIN_W / 2.0f, objectiveY});
        window.draw(inter);

        std::string next = "PROCHAINE VAGUE DANS " + fmtTenths(level.intermissionLeft()) + "s";
        sf::Text nextT(font, next, 16);
        nextT.setFillColor(sf::Color(200, 200, 200));
        centerText(nextT, {WIN_W / 2.0f, objectiveY + 22.0f});
        window.draw(nextT);
        return;
    }

    std::string header = "VAGUE " + std::to_string(level.waveIndex() + 1) + "  OBJECTIF";
    sf::Text h(font, header, 16);
    h.setFillColor(sf::Color(220, 220, 220));
    centerText(h, {WIN_W / 2.0f, objectiveY});
    window.draw(h);

    std::string obj;
    if (level.objectiveType() == LevelManager::ObjectiveType::Kill) {
        obj = "ELIMINER " + std::to_string(level.killsInWave()) + " / " + std::to_string(level.killTarget());
    } else {
        obj = "SURVIVRE " + fmtTenths(level.waveTime()) + " / " + fmtTenths(level.surviveTarget()) + "s";
    }
    sf::Text ot(font, obj, 18);
    ot.setFillColor(sf::Color::White);
    centerText(ot, {WIN_W / 2.0f, objectiveY + 22.0f});
    window.draw(ot);
}

void drawLaserPanel(sf::RenderWindow& window, const sf::Font& font, float laserCharge, float laserTimer, float timeLeft) {
    float panelW = 170.0f;
    float panelH = 44.0f;
    float x = WIN_W - panelW - 14.0f;
    float y = 12.0f;

    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({x, y});
    panel.setFillColor(sf::Color(25, 25, 30, 180));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(laserTimer > 0.0f ? sf::Color(160, 255, 180) : sf::Color(80, 90, 110));
    window.draw(panel);

    sf::Text title(font, "LASER", 14);
    title.setFillColor(sf::Color(225, 240, 230));
    title.setPosition({x + 8.0f, y + 5.0f});
    window.draw(title);

    float fillRatio = std::clamp(laserCharge, 0.0f, 1.0f);
    if (laserTimer > 0.0f) fillRatio = std::clamp(laserTimer / LASER_DURATION, 0.0f, 1.0f);
    sf::RectangleShape barBg({panelW - 16.0f, 12.0f});
    barBg.setPosition({x + 8.0f, y + 24.0f});
    barBg.setFillColor(sf::Color(45, 50, 60));
    window.draw(barBg);

    sf::RectangleShape barFill({(panelW - 16.0f) * fillRatio, 12.0f});
    barFill.setPosition({x + 8.0f, y + 24.0f});
    barFill.setFillColor(laserTimer > 0.0f ? sf::Color(120, 255, 170) : sf::Color(80, 200, 255));
    window.draw(barFill);

    std::string state = laserTimer > 0.0f ? ("ACTIF " + fmtTenths(laserTimer) + "s") : (fillRatio >= 1.0f ? "PRET (ESPACE)" : (std::to_string(static_cast<int>(fillRatio * 100.0f)) + "%"));
    if (timeLeft < LASER_TIME_COST && laserTimer <= 0.0f && fillRatio >= 1.0f) state = "TEMPS INSUFFISANT";
    sf::Text text(font, state, 11);
    text.setFillColor(sf::Color(230, 230, 230));
    text.setPosition({x + 62.0f, y + 6.0f});
    window.draw(text);
}
}

void hudDrawBuffsObjectives(
    sf::RenderWindow& window,
    const sf::Font& font,
    float timeLeft,
    float speedBoost,
    float rapidFire,
    float pierce,
    float laserCharge,
    float laserTimer,
    int score,
    int comboCount,
    const LevelManager& level
) {
    float barWidth = 300.0f;
    float barHeight = 8.0f;
    float barX = WIN_W / 2.0f - barWidth / 2.0f;
    float barY = 55.0f;
    float iconSize = 28.0f;
    float iconGap = 8.0f;
    float iconX = barX;
    float iconY = barY + barHeight + 10.0f;

    drawBuff(window, font, BonusType::Speed, speedBoost, iconY, iconSize, iconGap, iconX);
    drawBuff(window, font, BonusType::RapidFire, rapidFire, iconY, iconSize, iconGap, iconX);
    drawBuff(window, font, BonusType::Pierce, pierce, iconY, iconSize, iconGap, iconX);

    drawObjectives(window, font, iconY + iconSize + 26.0f, level);

    sf::Text sc(font, "Score: " + std::to_string(score), 24);
    sc.setFillColor(sf::Color::White);
    sc.setPosition({10.0f, 10.0f});
    window.draw(sc);

    if (comboCount >= 3) {
        sf::Text ct(font, "COMBO x" + std::to_string(comboCount), 20);
        ct.setFillColor(sf::Color(255, 200, 50));
        ct.setPosition({10.0f, 40.0f});
        window.draw(ct);
    }

    drawLaserPanel(window, font, laserCharge, laserTimer, timeLeft);
}

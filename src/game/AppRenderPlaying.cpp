#include "game/AppFlow.hpp"
#include "game/EffectsRenderer.hpp"
#include "game/RenderHelpers.hpp"
#include "game/HudRenderer.hpp"
#include "utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace {
constexpr float LASER_RENDER_RANGE = 2200.0f;

void drawLaser(App& a) {
    if (a.laserTimer <= 0.0f) return;
    sf::Vector2f dir = vecNormalize(a.laserDirection);
    if (dir.x == 0.0f && dir.y == 0.0f) return;

    float angle = std::atan2(dir.y, dir.x) * 180.0f / 3.14159265f;
    float t = std::clamp(a.laserTimer / LASER_DURATION, 0.0f, 1.0f);
    uint8_t glowAlpha = static_cast<uint8_t>(120.0f + 80.0f * t);
    uint8_t coreAlpha = static_cast<uint8_t>(190.0f + 60.0f * t);

    sf::Vector2f origin = a.player.getPosition();
    sf::RectangleShape glow({LASER_RENDER_RANGE, LASER_WIDTH * 1.8f});
    glow.setOrigin({0.0f, glow.getSize().y * 0.5f});
    glow.setPosition(origin);
    glow.setRotation(sf::degrees(angle));
    glow.setFillColor(sf::Color(90, 255, 170, glowAlpha));
    a.window.draw(glow);

    sf::RectangleShape core({LASER_RENDER_RANGE, LASER_WIDTH});
    core.setOrigin({0.0f, core.getSize().y * 0.5f});
    core.setPosition(origin);
    core.setRotation(sf::degrees(angle));
    core.setFillColor(sf::Color(220, 255, 220, coreAlpha));
    a.window.draw(core);
}
}

void appRenderPlaying(App& a) {
    sf::Color bg(20,15,25); if (a.timeLeft < 30.0f) { float pulse = (std::sin(a.gameTime*8.0f)+1.0f)/2.0f; bg.r = static_cast<uint8_t>(20 + pulse*30); }
    a.window.clear(bg);
    sf::View view = a.window.getDefaultView();
    sf::Vector2f shakeOffset{0.0f,0.0f}; if (a.shakeTimer>0.0f) { shakeOffset.x = static_cast<float>((std::rand()%100)-50)/50.0f * SHAKE_INTENSITY; shakeOffset.y = static_cast<float>((std::rand()%100)-50)/50.0f * SHAKE_INTENSITY; }
    view.setCenter(view.getCenter() + shakeOffset); a.window.setView(view);
    a.bonusSystem.draw(a.window, a.gameTime);
    drawShockwaves(a.window, a.shockwaves);
    for (auto& p : a.particles) a.window.draw(p.shape);
    for (auto& e : a.enemies) { float s = std::min(1.0f, e.spawnAge / ENEMY_SPAWN_DURATION); e.shape.setScale({s,s}); drawEnemy(a.window, e, a.player.getPosition()); }
    for (auto& ep : a.enemyProjectiles) a.window.draw(ep.shape);
    for (auto& b : a.bullets) a.window.draw(b.shape);
    drawLaser(a);
    drawTomato(a.window, a.player);
    drawFloatingTexts(a.window, a.fontLoaded ? &a.font : nullptr, a.floatingTexts);
    if (a.flashAlpha > 0.0f) { a.window.setView(a.window.getDefaultView()); drawFlashOverlay(a.window, a.flashAlpha); view.setCenter(view.getCenter()); a.window.setView(view); }
    a.window.setView(a.window.getDefaultView()); if (a.fontLoaded) drawHud(a.window, a.font, a.timeLeft, a.score, a.comboCount, a.speedBoostTimer, a.rapidFireTimer, a.pierceTimer, a.laserCharge, a.laserTimer, a.level);
    a.window.display();
}

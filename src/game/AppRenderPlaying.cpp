#include "game/AppFlow.hpp"
#include "game/EffectsRenderer.hpp"
#include "game/RenderHelpers.hpp"
#include "game/HudRenderer.hpp"
#include <algorithm>
#include <cmath>

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
    for (auto& b : a.bullets) a.window.draw(b.shape);
    drawTomato(a.window, a.player);
    drawFloatingTexts(a.window, a.fontLoaded ? &a.font : nullptr, a.floatingTexts);
    if (a.flashAlpha > 0.0f) { a.window.setView(a.window.getDefaultView()); drawFlashOverlay(a.window, a.flashAlpha); view.setCenter(view.getCenter()); a.window.setView(view); }
    a.window.setView(a.window.getDefaultView()); if (a.fontLoaded) drawHud(a.window, a.font, a.timeLeft, a.score, a.comboCount, a.speedBoostTimer, a.rapidFireTimer, a.level);
    a.window.display();
}

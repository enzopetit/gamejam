#include "game/AppFlow.hpp"
#include "game/EffectsUpdate.hpp"
#include "game/ParticleEffects.hpp"

void updatePost(App& a, float rawDt) {
    if (a.level.isWaveActive()) {
        a.bonusSystem.update(rawDt, a.player.getPosition(), PLAYER_RADIUS, a.timeLeft, a.speedBoostTimer, a.rapidFireTimer, a.pierceTimer, 0.0f);
        for (const auto& p : a.bonusSystem.getPickups()) spawnParticles(a.particles, p, 12, sf::Color(255,255,200));
        a.bonusSystem.clearPickups();
    }
    if (a.flashAlpha > 0.0f) { a.flashAlpha -= rawDt * (FLASH_MAX_ALPHA / FLASH_DURATION); if (a.flashAlpha < 0.0f) a.flashAlpha = 0.0f; }
    updateShockwaves(a.shockwaves, rawDt); updateFloatingTexts(a.floatingTexts, rawDt); updateParticles(a.particles, rawDt);
    eraseDead(a.bullets); eraseDead(a.enemies); eraseDead(a.enemyProjectiles); eraseDead(a.particles); eraseDead(a.shockwaves); eraseDead(a.floatingTexts);
    if (a.level.consumeWaveCompleted()) { a.bullets.clear(); a.enemies.clear(); a.enemyProjectiles.clear(); a.particles.clear(); a.shockwaves.clear(); a.floatingTexts.clear(); a.bonusSystem.clear(); a.comboCount=0; a.comboTimer=0.0f; a.spawnTimer=0.0f; }
    if (a.shakeTimer > 0.0f) a.shakeTimer -= rawDt;
}

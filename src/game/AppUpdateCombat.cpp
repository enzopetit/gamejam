#include "game/AppFlow.hpp"
#include "utils/Math.hpp"
#include "game/ParticleEffects.hpp"

void updateEnemiesAndCollisions(App& a, float rawDt, float dt) {
    if (!a.level.isWaveActive()) return;
    sf::Vector2f playerPos = a.player.getPosition();
    for (auto& e : a.enemies) {
        e.spawnAge += rawDt; sf::Vector2f ePos = e.shape.getPosition(); sf::Vector2f dir = vecNormalize(playerPos - ePos);
        ePos += dir * e.speed * dt; e.shape.setPosition(ePos);
        if (e.flashTimer > 0.0f) { e.flashTimer -= dt; if (e.flashTimer <= 0.0f) e.shape.setFillColor(e.baseColor);} 
        float collDist = PLAYER_RADIUS + e.shape.getRadius();
        if (distanceSq(ePos, playerPos) < collDist * collDist) { a.timeLeft -= 3.0f; e.alive=false; a.shakeTimer = SHAKE_DURATION; spawnParticles(a.particles, ePos, 15, sf::Color(200,50,50)); }
    }
    a.comboTimer -= dt; if (a.comboTimer <= 0.0f) a.comboCount = 0;
    for (auto& b : a.bullets) {
        if (!b.alive) continue; sf::Vector2f bPos = b.shape.getPosition();
        for (auto& e : a.enemies) {
            if (!e.alive) continue; float collDist = BULLET_RADIUS + e.shape.getRadius();
            if (distanceSq(bPos, e.shape.getPosition()) < collDist * collDist) {
                b.alive=false; e.hp -= 1.0f; e.flashTimer = 0.08f; e.shape.setFillColor(sf::Color::White); a.shakeTimer = SHAKE_DURATION * 0.5f; spawnParticles(a.particles, bPos, 8, sf::Color(220,50,30));
                if (e.hp <= 0.0f) {
                    e.alive=false; a.comboCount++; a.comboTimer=1.0f; float dropValue=e.timeDrop; if (a.comboCount>=3) dropValue *= 1.0f + (a.comboCount-2)*0.25f;
                    int killScore = static_cast<int>(10 * e.maxHp * (1.0f + a.comboCount * 0.1f)); a.score += killScore; spawnParticles(a.particles, e.shape.getPosition(), 20, e.baseColor);
                    bool isBig = e.maxHp >= 5; a.hitstopTimer = isBig ? HITSTOP_BIG : HITSTOP_SMALL; if (a.comboCount >= SLOWMO_COMBO_THRESHOLD || isBig) a.slowMoTimer = SLOWMO_DURATION; a.flashAlpha = FLASH_MAX_ALPHA;
                    Shockwave sw; sw.position = e.shape.getPosition(); sw.radius = e.shape.getRadius(); sw.maxRadius = SHOCKWAVE_MAX_RADIUS + (isBig?40.0f:0.0f); sw.maxLifetime=SHOCKWAVE_LIFETIME; sw.lifetime=sw.maxLifetime; sw.color=e.baseColor; a.shockwaves.push_back(sw);
                    if (a.fontLoaded) { FloatingText ft; ft.position = e.shape.getPosition() + sf::Vector2f(0.0f,-20.0f); ft.velocity={0.0f,-FLOATING_TEXT_SPEED}; ft.maxLifetime=FLOATING_TEXT_LIFETIME; ft.lifetime=ft.maxLifetime; ft.text = "+" + std::to_string(killScore); ft.color=sf::Color(255,255,100); ft.size=18; a.floatingTexts.push_back(ft);
                        if (a.comboCount>=3) { FloatingText ct; ct.position=e.shape.getPosition()+sf::Vector2f(0.0f,-40.0f); ct.velocity={0.0f,-FLOATING_TEXT_SPEED*0.8f}; ct.maxLifetime=FLOATING_TEXT_LIFETIME; ct.lifetime=ct.maxLifetime; ct.text = "x" + std::to_string(a.comboCount); ct.color=sf::Color(255,200,50); ct.size=22; a.floatingTexts.push_back(ct);} }
                    a.bonusSystem.spawnOnKill(e.shape.getPosition(), dropValue); a.level.onEnemyKilled();
                }
                break;
            }
        }
    }
}

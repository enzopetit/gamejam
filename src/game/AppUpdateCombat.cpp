#include "game/AppFlow.hpp"
#include "game/ParticleEffects.hpp"
#include "utils/Math.hpp"

#include <algorithm>

namespace {
constexpr float LASER_RANGE = 2200.0f;

float dot(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

float laserDistanceSq(sf::Vector2f point, sf::Vector2f origin, sf::Vector2f dir, float range) {
    sf::Vector2f rel = point - origin;
    float proj = std::clamp(dot(rel, dir), 0.0f, range);
    sf::Vector2f closest = origin + dir * proj;
    return distanceSq(point, closest);
}

void onEnemyKilled(App& a, Enemy& e) {
    e.alive = false;
    a.comboCount++;
    a.comboTimer = 1.0f;
    float dropValue = e.timeDrop;
    if (a.comboCount >= 3) dropValue *= 1.0f + (a.comboCount - 2) * 0.25f;
    int killScore = static_cast<int>(10 * e.maxHp * (1.0f + a.comboCount * 0.1f));
    a.score += killScore;
    spawnParticles(a.particles, e.shape.getPosition(), 20, e.baseColor);

    bool isBig = e.maxHp >= 5;
    a.hitstopTimer = isBig ? HITSTOP_BIG : HITSTOP_SMALL;
    if (a.comboCount >= SLOWMO_COMBO_THRESHOLD || isBig) a.slowMoTimer = SLOWMO_DURATION;
    a.flashAlpha = FLASH_MAX_ALPHA;

    Shockwave sw;
    sw.position = e.shape.getPosition();
    sw.radius = e.shape.getRadius();
    sw.maxRadius = SHOCKWAVE_MAX_RADIUS + (isBig ? 40.0f : 0.0f);
    sw.maxLifetime = SHOCKWAVE_LIFETIME;
    sw.lifetime = sw.maxLifetime;
    sw.color = e.baseColor;
    a.shockwaves.push_back(sw);

    if (a.fontLoaded) {
        FloatingText ft;
        ft.position = e.shape.getPosition() + sf::Vector2f(0.0f, -20.0f);
        ft.velocity = {0.0f, -FLOATING_TEXT_SPEED};
        ft.maxLifetime = FLOATING_TEXT_LIFETIME;
        ft.lifetime = ft.maxLifetime;
        ft.text = "+" + std::to_string(killScore);
        ft.color = sf::Color(255, 255, 100);
        ft.size = 18;
        a.floatingTexts.push_back(ft);

        if (a.comboCount >= 3) {
            FloatingText ct;
            ct.position = e.shape.getPosition() + sf::Vector2f(0.0f, -40.0f);
            ct.velocity = {0.0f, -FLOATING_TEXT_SPEED * 0.8f};
            ct.maxLifetime = FLOATING_TEXT_LIFETIME;
            ct.lifetime = ct.maxLifetime;
            ct.text = "x" + std::to_string(a.comboCount);
            ct.color = sf::Color(255, 200, 50);
            ct.size = 22;
            a.floatingTexts.push_back(ct);
        }
    }

    a.bonusSystem.spawnOnKill(e.shape.getPosition(), dropValue);
    a.level.onEnemyKilled();
}

void updateEnemyMotion(Enemy& e, sf::Vector2f playerPos, float rawDt, float dt) {
    e.spawnAge += rawDt;
    sf::Vector2f ePos = e.shape.getPosition();
    sf::Vector2f dir = vecNormalize(playerPos - ePos);
    ePos += dir * e.speed * dt;
    e.shape.setPosition(ePos);
    if (e.flashTimer > 0.0f) {
        e.flashTimer -= dt;
        if (e.flashTimer <= 0.0f) e.shape.setFillColor(e.baseColor);
    }
}

void collideEnemyPlayer(App& a, Enemy& e, sf::Vector2f playerPos) {
    float collDist = PLAYER_RADIUS + e.shape.getRadius();
    if (distanceSq(e.shape.getPosition(), playerPos) >= collDist * collDist) return;
    a.timeLeft -= 3.0f;
    e.alive = false;
    a.shakeTimer = SHAKE_DURATION;
    spawnParticles(a.particles, e.shape.getPosition(), 15, sf::Color(200, 50, 50));
}

bool isBulletEnemyCollision(const Bullet& b, const Enemy& e) {
    float collDist = BULLET_RADIUS + e.shape.getRadius();
    return distanceSq(b.shape.getPosition(), e.shape.getPosition()) < collDist * collDist;
}

void onBulletHit(App& a, Bullet& b, Enemy& e) {
    sf::Vector2f bPos = b.shape.getPosition();
    if (!b.piercing) b.alive = false;
    e.hp -= b.damage;
    e.flashTimer = 0.08f;
    e.shape.setFillColor(sf::Color::White);
    a.shakeTimer = SHAKE_DURATION * 0.5f;
    spawnParticles(a.particles, bPos, 8, sf::Color(220, 50, 30));

    if (b.piercing) {
        sf::Vector2f shift = vecNormalize(b.velocity) * (e.shape.getRadius() + BULLET_RADIUS + 1.0f);
        b.shape.setPosition(bPos + shift);
    }

    if (e.hp <= 0.0f) onEnemyKilled(a, e);
}

void updateBulletCollisions(App& a) {
    for (auto& b : a.bullets) {
        if (!b.alive) continue;
        for (auto& e : a.enemies) {
            if (!e.alive || !isBulletEnemyCollision(b, e)) continue;
            onBulletHit(a, b, e);
            break;
        }
    }
}

void applyLaserDamage(App& a, float dt, sf::Vector2f playerPos) {
    if (a.laserTimer <= 0.0f) return;
    sf::Vector2f dir = vecNormalize(a.laserDirection);
    if (dir.x == 0.0f && dir.y == 0.0f) return;

    float damagePerSecond = LASER_DAMAGE_PER_SEC + static_cast<float>(a.level.waveIndex()) * LASER_DAMAGE_PER_WAVE;
    float beamRadius = LASER_WIDTH * 0.5f;
    for (auto& e : a.enemies) {
        if (!e.alive) continue;
        float hitRadius = beamRadius + e.shape.getRadius();
        if (laserDistanceSq(e.shape.getPosition(), playerPos, dir, LASER_RANGE) > hitRadius * hitRadius) continue;

        e.hp -= damagePerSecond * dt;
        e.flashTimer = 0.03f;
        e.shape.setFillColor(sf::Color(255, 230, 200));
        if (e.hp <= 0.0f) onEnemyKilled(a, e);
    }
}
}

void updateEnemiesAndCollisions(App& a, float rawDt, float dt) {
    if (!a.level.isWaveActive()) return;
    sf::Vector2f playerPos = a.player.getPosition();
    for (auto& e : a.enemies) {
        updateEnemyMotion(e, playerPos, rawDt, dt);
        collideEnemyPlayer(a, e, playerPos);
    }
    a.comboTimer -= dt;
    if (a.comboTimer <= 0.0f) a.comboCount = 0;
    updateBulletCollisions(a);
    applyLaserDamage(a, dt, playerPos);
}

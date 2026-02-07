#include "game/AppFlow.hpp"
#include <algorithm>
#include <cstdlib>
#include "game/EnemyFactory.hpp"

void updateSpawns(App& a, float dt) {
    if (!a.level.isWaveActive()) return;
    a.spawnTimer += dt;
    float difficultyScale = 1.0f + static_cast<float>(a.level.waveIndex()) * 0.1f;
    float pressureBoost = (1.0f - std::clamp(a.timeLeft / TOTAL_TIME, 0.0f, 1.0f)) * TIME_LEFT_SPAWN_BOOST;
    float timePressure = std::clamp((TOTAL_TIME - a.timeLeft) / TOTAL_TIME, 0.0f, 1.0f) * difficultyScale;
    float spawnBias = static_cast<float>(a.level.waveIndex()) * 0.05f;
    a.spawnInterval = std::max(SPAWN_INTERVAL_MIN, SPAWN_INTERVAL_START - a.level.waveTime()/60.0f - spawnBias - pressureBoost);
    if (a.spawnTimer >= a.spawnInterval) {
        a.spawnTimer = 0.0f;
        int waveBonus = a.level.waveIndex() / 3; int pressureBonus = static_cast<int>(timePressure * 2.0f);
        int spawnCount = std::clamp(1 + waveBonus + pressureBonus, 1, 5);
        for (int i = 0; i < spawnCount; ++i) {
            int roll = std::rand() % 100; int type;
            if (a.level.waveIndex() < 2) type = 0; else if (a.level.waveIndex() < 4) type = (roll < 70) ? 0 : 1; else type = (roll < 50) ? 0 : (roll < 85 ? 1 : 2);
            float difficultyTime = a.level.waveTime() + static_cast<float>(a.level.waveIndex()) * 20.0f;
            a.enemies.push_back(createEnemy(type, difficultyTime));
        }
    }
}

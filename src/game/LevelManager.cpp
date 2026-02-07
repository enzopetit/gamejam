#include "game/LevelManager.hpp"

#include "game/GameConfig.hpp"

LevelManager::LevelManager() {
    reset();
}

void LevelManager::reset() {
    currentWave = 0;
    kills = 0;
    waveTimer = 0.0f;
    intermissionTimer = 0.0f;
    waveCompletedFlag = false;
}

void LevelManager::update(float dt) {
    if (intermissionTimer > 0.0f) {
        intermissionTimer -= dt;
        if (intermissionTimer <= 0.0f) {
            intermissionTimer = 0.0f;
            currentWave++;
            kills = 0;
            waveTimer = 0.0f;
        }
        return;
    }
    waveTimer += dt;
    if (!waveCompletedFlag && objectiveType() == ObjectiveType::Survive && waveTimer >= surviveTarget()) {
        waveCompletedFlag = true;
        intermissionTimer = WAVE_INTERMISSION;
    }
}

void LevelManager::onEnemyKilled() {
    if (intermissionTimer > 0.0f) return;
    if (objectiveType() != ObjectiveType::Kill) return;
    kills++;
    if (kills >= killTarget()) {
        waveCompletedFlag = true;
        intermissionTimer = WAVE_INTERMISSION;
    }
}

bool LevelManager::isWaveActive() const {
    return intermissionTimer <= 0.0f;
}

bool LevelManager::isIntermission() const {
    return intermissionTimer > 0.0f;
}

bool LevelManager::consumeWaveCompleted() {
    if (!waveCompletedFlag) return false;
    waveCompletedFlag = false;
    return true;
}

// Query methods implemented in LevelManager_Queries.cpp

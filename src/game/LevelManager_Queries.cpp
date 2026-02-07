#include "game/LevelManager.hpp"
#include "game/GameConfig.hpp"

int LevelManager::waveIndex() const {
    return currentWave;
}

int LevelManager::killTarget() const {
    if (isBossWave()) return 1;
    return WAVE_KILL_BASE + currentWave * WAVE_KILL_STEP;
}

int LevelManager::killsInWave() const {
    return kills;
}

float LevelManager::waveTime() const {
    return waveTimer;
}

float LevelManager::intermissionLeft() const {
    return intermissionTimer;
}

float LevelManager::surviveTarget() const {
    return WAVE_SURVIVE_BASE + static_cast<float>(currentWave) * WAVE_SURVIVE_STEP;
}

LevelManager::ObjectiveType LevelManager::objectiveType() const {
    if (isBossWave()) return ObjectiveType::Kill;
    return (currentWave % 2 == 0) ? ObjectiveType::Kill : ObjectiveType::Survive;
}

bool LevelManager::isBossWave() const {
    return (currentWave + 1) % BOSS_WAVE_INTERVAL == 0;
}

int LevelManager::bossKillTarget() const {
    return 1;
}


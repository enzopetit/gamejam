#pragma once

class LevelManager {
public:
    LevelManager();

    void reset();
    void update(float dt);
    void onEnemyKilled();

    enum class ObjectiveType {
        Kill,
        Survive
    };

    bool isWaveActive() const;
    bool isIntermission() const;
    bool consumeWaveCompleted();

    int waveIndex() const;
    int killTarget() const;
    int killsInWave() const;
    float waveTime() const;
    float intermissionLeft() const;
    float surviveTarget() const;
    ObjectiveType objectiveType() const;
    bool isBossWave() const;
    int bossKillTarget() const;

private:
    int currentWave;
    int kills;
    float waveTimer;
    float intermissionTimer;
    bool waveCompletedFlag;
};

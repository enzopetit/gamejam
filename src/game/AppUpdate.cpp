#include "game/AppFlow.hpp"

void appUpdatePlaying(App& a, float rawDt, float dt) {
    a.level.update(dt); a.music.update(a.timeLeft, dt);
    a.gameTime += rawDt; if (a.level.isWaveActive()) { a.timeLeft -= dt; if (a.timeLeft <= 0.0f) { a.timeLeft = 0.0f; a.state = AppState::GameOver; return; } }
    if (a.hitstopTimer > 0.0f) { a.hitstopTimer -= rawDt; dt = 0.0f; } else if (a.slowMoTimer > 0.0f) { a.slowMoTimer -= rawDt; dt *= SLOWMO_SCALE; }
    updateMotionAndShooting(a, rawDt, dt);
    updateSpawns(a, dt);
    updateEnemiesAndCollisions(a, rawDt, dt);
    updatePost(a, rawDt);
}


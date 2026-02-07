#pragma once

struct GameStats {
    int totalKills = 0;
    int bestCombo = 0;
    int shotsFired = 0;
    int shotsHit = 0;
    float accuracy() const {
        return shotsFired > 0 ? static_cast<float>(shotsHit) / static_cast<float>(shotsFired) * 100.0f : 0.0f;
    }
};


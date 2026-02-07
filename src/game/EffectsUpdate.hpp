#pragma once

#include "actors/Actor.hpp"

#include <vector>
#include <algorithm>

void updateParticles(std::vector<Particle>& particles, float rawDt);
void updateShockwaves(std::vector<Shockwave>& shockwaves, float rawDt);
void updateFloatingTexts(std::vector<FloatingText>& texts, float rawDt);

template <typename T>
inline void eraseDead(std::vector<T>& v) {
    v.erase(std::remove_if(v.begin(), v.end(), [](const T& e) { return !e.alive; }), v.end());
}

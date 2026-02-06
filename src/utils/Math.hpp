#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

inline float vecLength(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline sf::Vector2f vecNormalize(sf::Vector2f v) {
    float len = vecLength(v);
    if (len == 0.0f) return {0.0f, 0.0f};
    return {v.x / len, v.y / len};
}

inline float distanceSq(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

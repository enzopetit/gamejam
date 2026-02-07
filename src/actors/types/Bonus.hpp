#pragma once

#include <SFML/Graphics.hpp>

enum class BonusType {
    Time,
    Speed,
    RapidFire,
    Pierce
};

struct Bonus {
    sf::CircleShape shape;
    float value;
    float lifetime;
    BonusType type;
    bool alive = true;
};


#pragma once

#include <SFML/Graphics.hpp>

struct Mine {
    sf::CircleShape shape;
    float radius;
    float lifetime;
    bool alive = true;
};


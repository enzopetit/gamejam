#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct FloatingText {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    std::string text;
    sf::Color color;
    unsigned int size;
    bool alive = true;
};


#pragma once

#include "actors/Actor.hpp"
#include "game/GameConfig.hpp"
#include "utils/Math.hpp"

#include <SFML/Graphics.hpp>

void drawTomato(sf::RenderWindow& window, const sf::CircleShape& body);

void drawEnemy(sf::RenderWindow& window, const Enemy& e, sf::Vector2f playerPos);

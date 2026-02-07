#pragma once

#include "actors/Actor.hpp"
#include "game/GameConfig.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

void spawnParticles(std::vector<Particle>& particles, sf::Vector2f pos, int count, sf::Color color);

void spawnBulletTrail(std::vector<Particle>& particles, sf::Vector2f pos, sf::Color color);

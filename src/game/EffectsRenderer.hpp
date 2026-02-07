#pragma once

#include "actors/Actor.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

void drawShockwaves(sf::RenderWindow& window, const std::vector<Shockwave>& shockwaves);
void drawFlashOverlay(sf::RenderWindow& window, float alpha);
void drawFloatingTexts(sf::RenderWindow& window, const sf::Font* font, const std::vector<FloatingText>& texts);

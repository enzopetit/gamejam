#include "game/RenderHelpers.hpp"

void drawTomato(sf::RenderWindow& window, const sf::CircleShape& body) {
    sf::Vector2f pos = body.getPosition(); sf::Vector2f sc = body.getScale(); float r = PLAYER_RADIUS;
    window.draw(body);
    sf::CircleShape shade(r * 0.7f); shade.setFillColor(sf::Color(180,30,30)); shade.setOrigin({r*0.7f,r*0.7f}); shade.setPosition({pos.x + r*0.15f*sc.x, pos.y + r*0.2f*sc.y}); shade.setScale(sc); window.draw(shade);
    sf::CircleShape h1(r*0.25f); h1.setFillColor(sf::Color(255,130,130,160)); h1.setOrigin({r*0.25f,r*0.25f}); h1.setPosition({pos.x - r*0.35f*sc.x, pos.y - r*0.35f*sc.y}); h1.setScale(sc); window.draw(h1);
    sf::CircleShape h2(r*0.12f); h2.setFillColor(sf::Color(255,180,180,120)); h2.setOrigin({r*0.12f,r*0.12f}); h2.setPosition({pos.x - r*0.2f*sc.x, pos.y - r*0.5f*sc.y}); h2.setScale(sc); window.draw(h2);
    sf::RectangleShape stem({r*0.15f, r*0.4f}); stem.setFillColor(sf::Color(80,140,40)); stem.setOrigin({r*0.075f, r*0.4f}); stem.setPosition({pos.x, pos.y - r*0.85f*sc.y}); stem.setScale(sc); window.draw(stem);
    sf::ConvexShape leaf1(4); leaf1.setPoint(0,{0,0}); leaf1.setPoint(1,{r*0.5f,-r*0.15f}); leaf1.setPoint(2,{r*0.55f,-r*0.05f}); leaf1.setPoint(3,{r*0.1f,r*0.1f}); leaf1.setFillColor(sf::Color(60,160,40)); leaf1.setPosition({pos.x, pos.y - r*0.85f*sc.y}); leaf1.setScale(sc); window.draw(leaf1);
    sf::ConvexShape leaf2(4); leaf2.setPoint(0,{0,0}); leaf2.setPoint(1,{-r*0.45f,-r*0.2f}); leaf2.setPoint(2,{-r*0.5f,-r*0.08f}); leaf2.setPoint(3,{-r*0.08f,r*0.1f}); leaf2.setFillColor(sf::Color(50,150,35)); leaf2.setPosition({pos.x, pos.y - r*0.85f*sc.y}); leaf2.setScale(sc); window.draw(leaf2);
    sf::ConvexShape leaf3(4); leaf3.setPoint(0,{0,0}); leaf3.setPoint(1,{r*0.3f,-r*0.3f}); leaf3.setPoint(2,{r*0.35f,-r*0.18f}); leaf3.setPoint(3,{r*0.05f,r*0.05f}); leaf3.setFillColor(sf::Color(70,170,45)); leaf3.setPosition({pos.x - r*0.05f, pos.y - r*0.9f*sc.y}); leaf3.setScale(sc); window.draw(leaf3);
}


#include "game/RenderHelpers.hpp"

#include <cmath>

namespace {
bool isSmallType(EnemyType type) {
    return type == EnemyType::Caterpillar || type == EnemyType::Ant;
}

bool isMediumType(EnemyType type) {
    return type == EnemyType::Wasp || type == EnemyType::Spider;
}
}

void drawEnemy(sf::RenderWindow& window, const Enemy& e, sf::Vector2f playerPos) {
    sf::Vector2f ePos = e.shape.getPosition(); sf::Vector2f sc = e.shape.getScale(); float r = e.shape.getRadius();
    sf::Vector2f dir = vecNormalize(playerPos - ePos); sf::Vector2f perp = {-dir.y, dir.x};

    if (e.type == EnemyType::Mosquito) {
        window.draw(e.shape);
        sf::ConvexShape trompe(3);
        trompe.setPoint(0, {-perp.x*r*0.1f, -perp.y*r*0.1f});
        trompe.setPoint(1, {perp.x*r*0.1f, perp.y*r*0.1f});
        trompe.setPoint(2, {dir.x*r*1.2f, dir.y*r*1.2f});
        trompe.setFillColor(sf::Color(100, 100, 110));
        trompe.setPosition(ePos);
        trompe.setScale(sc);
        window.draw(trompe);
        sf::ConvexShape wingL(3);
        wingL.setPoint(0, {0, 0});
        wingL.setPoint(1, {perp.x*r*1.5f + dir.x*r*0.3f, perp.y*r*1.5f + dir.y*r*0.3f});
        wingL.setPoint(2, {perp.x*r*0.8f - dir.x*r*0.5f, perp.y*r*0.8f - dir.y*r*0.5f});
        wingL.setFillColor(sf::Color(200, 200, 220, 60));
        wingL.setOutlineColor(sf::Color(180, 180, 200, 100));
        wingL.setOutlineThickness(1.0f);
        wingL.setPosition(ePos);
        wingL.setScale(sc);
        window.draw(wingL);
        sf::ConvexShape wingR(3);
        wingR.setPoint(0, {0, 0});
        wingR.setPoint(1, {-perp.x*r*1.5f + dir.x*r*0.3f, -perp.y*r*1.5f + dir.y*r*0.3f});
        wingR.setPoint(2, {-perp.x*r*0.8f - dir.x*r*0.5f, -perp.y*r*0.8f - dir.y*r*0.5f});
        wingR.setFillColor(sf::Color(200, 200, 220, 60));
        wingR.setOutlineColor(sf::Color(180, 180, 200, 100));
        wingR.setOutlineThickness(1.0f);
        wingR.setPosition(ePos);
        wingR.setScale(sc);
        window.draw(wingR);
        return;
    }

    if (e.type == EnemyType::Firefly) {
        float pulse = 0.5f + 0.5f * std::sin(e.phaseTimer * 5.0f);
        sf::CircleShape halo(r * 2.0f);
        halo.setOrigin({r * 2.0f, r * 2.0f});
        halo.setPosition(ePos);
        halo.setFillColor(sf::Color(255, 255, 100, static_cast<std::uint8_t>(40 * pulse)));
        halo.setScale(sc);
        window.draw(halo);
        window.draw(e.shape);
        sf::CircleShape glow(r * 0.6f);
        glow.setOrigin({r * 0.6f, r * 0.6f});
        glow.setPosition(ePos - dir * r * 0.5f);
        glow.setFillColor(sf::Color(255, 255, 180, static_cast<std::uint8_t>(150 + 105 * pulse)));
        glow.setScale(sc);
        window.draw(glow);
        return;
    }

    if (e.type == EnemyType::Scorpion) {
        window.draw(e.shape);
        sf::CircleShape seg1(r * 0.5f);
        seg1.setOrigin({r * 0.5f, r * 0.5f});
        seg1.setFillColor(sf::Color(120, 75, 30));
        seg1.setPosition(ePos - dir * r * 1.1f);
        seg1.setScale(sc);
        window.draw(seg1);
        sf::CircleShape seg2(r * 0.4f);
        seg2.setOrigin({r * 0.4f, r * 0.4f});
        seg2.setFillColor(sf::Color(110, 65, 25));
        seg2.setPosition(ePos - dir * r * 1.8f);
        seg2.setScale(sc);
        window.draw(seg2);
        sf::ConvexShape dard(3);
        dard.setPoint(0, {-perp.x * r * 0.15f, -perp.y * r * 0.15f});
        dard.setPoint(1, {perp.x * r * 0.15f, perp.y * r * 0.15f});
        dard.setPoint(2, {-dir.x * r * 0.7f + (-dir.y) * r * 0.3f, -dir.y * r * 0.7f + dir.x * r * 0.3f});
        dard.setFillColor(sf::Color(60, 60, 60));
        dard.setPosition(ePos - dir * r * 2.1f);
        dard.setScale(sc);
        window.draw(dard);
        sf::ConvexShape pinceL(3);
        pinceL.setPoint(0, {0, 0});
        pinceL.setPoint(1, {dir.x * r * 0.6f + perp.x * r * 0.4f, dir.y * r * 0.6f + perp.y * r * 0.4f});
        pinceL.setPoint(2, {dir.x * r * 0.3f + perp.x * r * 0.6f, dir.y * r * 0.3f + perp.y * r * 0.6f});
        pinceL.setFillColor(sf::Color(100, 55, 20));
        pinceL.setPosition(ePos + dir * r * 0.8f + perp * r * 0.3f);
        pinceL.setScale(sc);
        window.draw(pinceL);
        sf::ConvexShape pinceR(3);
        pinceR.setPoint(0, {0, 0});
        pinceR.setPoint(1, {dir.x * r * 0.6f - perp.x * r * 0.4f, dir.y * r * 0.6f - perp.y * r * 0.4f});
        pinceR.setPoint(2, {dir.x * r * 0.3f - perp.x * r * 0.6f, dir.y * r * 0.3f - perp.y * r * 0.6f});
        pinceR.setFillColor(sf::Color(100, 55, 20));
        pinceR.setPosition(ePos + dir * r * 0.8f - perp * r * 0.3f);
        pinceR.setScale(sc);
        window.draw(pinceR);
        return;
    }

    if (e.type == EnemyType::Ladybug) {
        float pulse = 0.5f + 0.5f * std::sin(e.phaseTimer * LADYBUG_PULSE_FREQ * 2.0f * 3.14159f);
        sf::CircleShape aura(r * 1.6f);
        aura.setOrigin({r * 1.6f, r * 1.6f});
        aura.setPosition(ePos);
        aura.setFillColor(sf::Color(255, 40, 40, static_cast<std::uint8_t>(30 + 30 * pulse)));
        aura.setScale(sc);
        window.draw(aura);
        window.draw(e.shape);
        sf::RectangleShape line({r * 0.08f, r * 2.0f});
        line.setOrigin({r * 0.04f, r * 1.0f});
        line.setFillColor(sf::Color::Black);
        line.setPosition(ePos);
        line.setScale(sc);
        window.draw(line);
        for (int i = 0; i < 4; ++i) {
            float offX = (i < 2) ? 0.35f : -0.35f;
            float offY = (i % 2 == 0) ? 0.3f : -0.3f;
            sf::CircleShape dot(r * 0.12f);
            dot.setOrigin({r * 0.12f, r * 0.12f});
            dot.setFillColor(sf::Color::Black);
            dot.setPosition(ePos + perp * r * offX + dir * r * offY);
            dot.setScale(sc);
            window.draw(dot);
        }
        sf::CircleShape head(r * 0.45f);
        head.setOrigin({r * 0.45f, r * 0.45f});
        head.setFillColor(sf::Color(30, 30, 30));
        head.setPosition(ePos + dir * r * 0.9f);
        head.setScale(sc);
        window.draw(head);
        return;
    }

    window.draw(e.shape);
    if (isSmallType(e.type)) {
        sf::CircleShape seg1(r*0.65f); seg1.setFillColor(sf::Color(60,150,60)); seg1.setOrigin({r*0.65f,r*0.65f}); seg1.setPosition(ePos - dir*r*0.9f*sc.x); seg1.setScale(sc); window.draw(seg1);
        sf::CircleShape seg2(r*0.5f); seg2.setFillColor(sf::Color(50,130,50)); seg2.setOrigin({r*0.5f,r*0.5f}); seg2.setPosition(ePos - dir*r*1.6f*sc.x); seg2.setScale(sc); window.draw(seg2);
        sf::CircleShape eyeL(r*0.22f); eyeL.setFillColor(sf::Color::White); eyeL.setOrigin({r*0.22f,r*0.22f}); eyeL.setPosition(ePos + dir*r*0.5f*sc.x + perp*r*0.35f*sc.x); eyeL.setScale(sc); window.draw(eyeL);
        sf::CircleShape eyeR(r*0.22f); eyeR.setFillColor(sf::Color::White); eyeR.setOrigin({r*0.22f,r*0.22f}); eyeR.setPosition(ePos + dir*r*0.5f*sc.x - perp*r*0.35f*sc.x); eyeR.setScale(sc); window.draw(eyeR);
        sf::CircleShape pupilL(r*0.12f); pupilL.setFillColor(sf::Color::Black); pupilL.setOrigin({r*0.12f,r*0.12f}); pupilL.setPosition(ePos + dir*r*0.6f*sc.x + perp*r*0.35f*sc.x); pupilL.setScale(sc); window.draw(pupilL);
        sf::CircleShape pupilR(r*0.12f); pupilR.setFillColor(sf::Color::Black); pupilR.setOrigin({r*0.12f,r*0.12f}); pupilR.setPosition(ePos + dir*r*0.6f*sc.x - perp*r*0.35f*sc.x); pupilR.setScale(sc); window.draw(pupilR);
        sf::RectangleShape antL({r*0.08f,r*0.6f}); antL.setFillColor(sf::Color(40,100,40)); antL.setOrigin({r*0.04f,r*0.6f}); antL.setPosition(ePos + dir*r*0.7f*sc.x + perp*r*0.25f*sc.x); antL.setScale(sc); window.draw(antL);
        sf::RectangleShape antR({r*0.08f,r*0.6f}); antR.setFillColor(sf::Color(40,100,40)); antR.setOrigin({r*0.04f,r*0.6f}); antR.setPosition(ePos + dir*r*0.7f*sc.x - perp*r*0.25f*sc.x); antR.setScale(sc); window.draw(antR);
    } else if (isMediumType(e.type)) {
        sf::CircleShape abdomen(r*0.8f); abdomen.setFillColor(sf::Color(220,200,50)); abdomen.setOrigin({r*0.8f,r*0.8f}); abdomen.setPosition(ePos - dir*r*1.0f*sc.x); abdomen.setScale(sc); window.draw(abdomen);
        sf::RectangleShape stripe1({r*1.4f,r*0.15f}); stripe1.setFillColor(sf::Color::Black); stripe1.setOrigin({r*0.7f,r*0.075f}); stripe1.setPosition(ePos - dir*r*0.8f*sc.x); stripe1.setScale(sc); window.draw(stripe1);
        sf::RectangleShape stripe2({r*1.2f,r*0.15f}); stripe2.setFillColor(sf::Color::Black); stripe2.setOrigin({r*0.6f,r*0.075f}); stripe2.setPosition(ePos - dir*r*1.2f*sc.x); stripe2.setScale(sc); window.draw(stripe2);
        sf::ConvexShape wingL(4); wingL.setPoint(0,{0,0}); wingL.setPoint(1,{perp.x*r*1.2f+dir.x*r*0.3f, perp.y*r*1.2f+dir.y*r*0.3f}); wingL.setPoint(2,{perp.x*r*1.0f-dir.x*r*0.4f, perp.y*r*1.0f-dir.y*r*0.4f}); wingL.setPoint(3,{-dir.x*r*0.2f,-dir.y*r*0.2f}); wingL.setFillColor(sf::Color(255,255,255,80)); wingL.setOutlineColor(sf::Color(200,200,200,120)); wingL.setOutlineThickness(1.0f); wingL.setPosition(ePos + dir*r*0.2f*sc.x); wingL.setScale(sc); window.draw(wingL);
        sf::ConvexShape wingR(4); wingR.setPoint(0,{0,0}); wingR.setPoint(1,{-perp.x*r*1.2f+dir.x*r*0.3f, -perp.y*r*1.2f+dir.y*r*0.3f}); wingR.setPoint(2,{-perp.x*r*1.0f-dir.x*r*0.4f, -perp.y*r*1.0f-dir.y*r*0.4f}); wingR.setPoint(3,{-dir.x*r*0.2f,-dir.y*r*0.2f}); wingR.setFillColor(sf::Color(255,255,255,80)); wingR.setOutlineColor(sf::Color(200,200,200,120)); wingR.setOutlineThickness(1.0f); wingR.setPosition(ePos + dir*r*0.2f*sc.x); wingR.setScale(sc); window.draw(wingR);
        sf::ConvexShape dard(3); dard.setPoint(0,{-perp.x*r*0.15f,-perp.y*r*0.15f}); dard.setPoint(1,{perp.x*r*0.15f,perp.y*r*0.15f}); dard.setPoint(2,{-dir.x*r*0.5f,-dir.y*r*0.5f}); dard.setFillColor(sf::Color(60,60,60)); dard.setPosition(ePos - dir*r*1.6f*sc.x); dard.setScale(sc); window.draw(dard);
        sf::CircleShape eyeL(r*0.2f); eyeL.setFillColor(sf::Color(200,30,30)); eyeL.setOrigin({r*0.2f,r*0.2f}); eyeL.setPosition(ePos + dir*r*0.55f*sc.x + perp*r*0.35f*sc.x); eyeL.setScale(sc); window.draw(eyeL);
        sf::CircleShape eyeR(r*0.2f); eyeR.setFillColor(sf::Color(200,30,30)); eyeR.setOrigin({r*0.2f,r*0.2f}); eyeR.setPosition(ePos + dir*r*0.55f*sc.x - perp*r*0.35f*sc.x); eyeR.setScale(sc); window.draw(eyeR);
    } else {
        sf::RectangleShape carapace({r*0.06f,r*1.6f}); carapace.setFillColor(sf::Color::Black); carapace.setOrigin({r*0.03f,r*0.8f}); carapace.setPosition(ePos); carapace.setScale(sc); window.draw(carapace);
        sf::ConvexShape mandL(3); mandL.setPoint(0,{0,0}); mandL.setPoint(1,{dir.x*r*0.5f+perp.x*r*0.25f, dir.y*r*0.5f+perp.y*r*0.25f}); mandL.setPoint(2,{dir.x*r*0.4f-perp.x*r*0.05f, dir.y*r*0.4f-perp.y*r*0.05f}); mandL.setFillColor(sf::Color(80,40,20)); mandL.setPosition(ePos + dir*r*0.8f*sc.x + perp*r*0.15f*sc.x); mandL.setScale(sc); window.draw(mandL);
        sf::ConvexShape mandR(3); mandR.setPoint(0,{0,0}); mandR.setPoint(1,{dir.x*r*0.5f-perp.x*r*0.25f, dir.y*r*0.5f-perp.y*r*0.25f}); mandR.setPoint(2,{dir.x*r*0.4f+perp.x*r*0.05f, dir.y*r*0.4f+perp.y*r*0.05f}); mandR.setFillColor(sf::Color(80,40,20)); mandR.setPosition(ePos + dir*r*0.8f*sc.x - perp*r*0.15f*sc.x); mandR.setScale(sc); window.draw(mandR);
        for (int i=0;i<3;++i){ float off=(i-1)*r*0.5f; sf::RectangleShape legL({r*0.6f,r*0.1f}); legL.setFillColor(sf::Color(60,30,15)); legL.setOrigin({0.0f,r*0.05f}); legL.setPosition(ePos + perp*r*0.85f*sc.x + dir*off*sc.x); legL.setScale(sc); window.draw(legL);
            sf::RectangleShape legR({r*0.6f,r*0.1f}); legR.setFillColor(sf::Color(60,30,15)); legR.setOrigin({r*0.6f,r*0.05f}); legR.setPosition(ePos - perp*r*0.85f*sc.x + dir*off*sc.x); legR.setScale(sc); window.draw(legR);} 
        sf::CircleShape eyeL(r*0.2f); eyeL.setFillColor(sf::Color(230,200,50)); eyeL.setOrigin({r*0.2f,r*0.2f}); eyeL.setPosition(ePos + dir*r*0.5f*sc.x + perp*r*0.35f*sc.x); eyeL.setScale(sc); window.draw(eyeL);
        sf::CircleShape eyeR(r*0.2f); eyeR.setFillColor(sf::Color(230,200,50)); eyeR.setOrigin({r*0.2f,r*0.2f}); eyeR.setPosition(ePos + dir*r*0.5f*sc.x - perp*r*0.35f*sc.x); eyeR.setScale(sc); window.draw(eyeR);
    }
}

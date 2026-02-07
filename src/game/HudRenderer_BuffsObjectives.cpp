#include "game/HudRenderer.hpp"
#include "game/GameConfig.hpp"
#include "actors/Actor.hpp"
#include <algorithm>
#include <cmath>

static std::string fmt(float v){ if (v<0.0f) v=0.0f; int tt = static_cast<int>(std::ceil(v*10.0f)); int w=tt/10, t=tt%10; return std::to_string(w)+"."+std::to_string(t);} 
static sf::Color buffFill(BonusType t){ if (t==BonusType::Speed) return sf::Color(80,200,255); if (t==BonusType::RapidFire) return sf::Color(255,200,80); return sf::Color(255,80,80);} 
static sf::Color buffOutline(BonusType t){ if (t==BonusType::Speed) return sf::Color(120,255,255); if (t==BonusType::RapidFire) return sf::Color(255,255,120); return sf::Color(255,200,50);} 
static std::string buffName(BonusType t){ if (t==BonusType::Speed) return "VITESSE"; if (t==BonusType::RapidFire) return "RAPIDE"; return "TEMPS";} 

void hudDrawBuffsObjectives(sf::RenderWindow& window, const sf::Font& font, float timeLeft, float speedBoost, float rapidFire, int score, int comboCount, const LevelManager& level) {
    float barWidth=300.0f, barHeight=8.0f, barX=WIN_W/2.0f - barWidth/2.0f, barY=55.0f; float iconSize=28.0f, iconGap=8.0f, iconX=barX, iconY=barY+barHeight+10.0f;
    auto drawBuff=[&](BonusType type, float rem){ if (rem<=0.0f) return; std::string name=buffName(type); sf::Text label(font, name, 12); label.setFillColor(sf::Color(230,230,230)); sf::FloatRect lb=label.getLocalBounds(); float slotW=std::max(iconSize, lb.size.x)+6.0f; float cx=iconX+slotW/2.0f; sf::RectangleShape icon({iconSize,iconSize}); icon.setFillColor(buffFill(type)); icon.setOutlineColor(buffOutline(type)); icon.setOutlineThickness(2.0f); icon.setPosition({cx-iconSize/2.0f, iconY}); window.draw(icon); sf::Text t(font, fmt(rem),14); t.setFillColor(sf::Color::Black); sf::FloatRect b=t.getLocalBounds(); t.setOrigin({b.position.x + b.size.x/2.0f, b.position.y + b.size.y/2.0f}); t.setPosition({cx, iconY+iconSize/2.0f}); window.draw(t); label.setOrigin({lb.position.x + lb.size.x/2.0f, lb.position.y + lb.size.y/2.0f}); label.setPosition({cx, iconY + iconSize + 10.0f}); window.draw(label); iconX += slotW + iconGap; };
    drawBuff(BonusType::Speed, speedBoost); drawBuff(BonusType::RapidFire, rapidFire);
    auto fwt=[&](float v){ if (v<0.0f) v=0.0f; int tt=static_cast<int>(std::ceil(v*10.0f)); int w=tt/10, t=tt%10; return std::to_string(w)+"."+std::to_string(t); };
    float objectiveY = iconY + iconSize + 26.0f;
    if (level.isIntermission()) {
        sf::Text inter(font, "VAGUE TERMINEE", 18); inter.setFillColor(sf::Color(230,230,230)); sf::FloatRect ib=inter.getLocalBounds(); inter.setOrigin({ib.position.x + ib.size.x/2.0f, ib.position.y + ib.size.y/2.0f}); inter.setPosition({WIN_W/2.0f, objectiveY}); window.draw(inter);
        std::string next = "PROCHAINE VAGUE DANS " + fwt(level.intermissionLeft()) + "s"; sf::Text nextT(font, next, 16); nextT.setFillColor(sf::Color(200,200,200)); sf::FloatRect nb=nextT.getLocalBounds(); nextT.setOrigin({nb.position.x + nb.size.x/2.0f, nb.position.y + nb.size.y/2.0f}); nextT.setPosition({WIN_W/2.0f, objectiveY + 22.0f}); window.draw(nextT);
    } else {
        std::string header = "VAGUE " + std::to_string(level.waveIndex()+1) + "  OBJECTIF"; sf::Text ht(font, header, 16); ht.setFillColor(sf::Color(220,220,220)); sf::FloatRect hb=ht.getLocalBounds(); ht.setOrigin({hb.position.x + hb.size.x/2.0f, hb.position.y + hb.size.y/2.0f}); ht.setPosition({WIN_W/2.0f, objectiveY}); window.draw(ht);
        std::string obj; if (level.objectiveType()==LevelManager::ObjectiveType::Kill) obj = "ELIMINER " + std::to_string(level.killsInWave()) + " / " + std::to_string(level.killTarget()); else obj = "SURVIVRE " + fwt(level.waveTime()) + " / " + fwt(level.surviveTarget()) + "s";
        sf::Text ot(font, obj, 18); ot.setFillColor(sf::Color::White); sf::FloatRect ob=ot.getLocalBounds(); ot.setOrigin({ob.position.x + ob.size.x/2.0f, ob.position.y + ob.size.y/2.0f}); ot.setPosition({WIN_W/2.0f, objectiveY + 22.0f}); window.draw(ot);
    }
    sf::Text sc(font, "Score: "+std::to_string(score), 24); sc.setFillColor(sf::Color::White); sc.setPosition({10.0f, 10.0f}); window.draw(sc);
    if (comboCount >= 3) { sf::Text ct(font, "COMBO x"+std::to_string(comboCount), 20); ct.setFillColor(sf::Color(255,200,50)); ct.setPosition({10.0f, 40.0f}); window.draw(ct); }
}

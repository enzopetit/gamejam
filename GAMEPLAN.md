# Tomato Blaster - Game Jam 24h

## Theme: "5 Minutes"
## Bonus: "Ajouter des tomates"

---

## Concept

Bullet hell / shoot'em up en vue top-down. Le joueur tire des tomates sur des vagues d'ennemis.
Le twist : **tirer consomme du temps** sur un chrono de 5 minutes. Tuer des ennemis drop des tomates
qui restaurent du temps. Si le chrono atteint 0 → Game Over.

---

## Controles

- **ZQSD / Fleches** : Deplacement
- **Souris** : Viser
- **Clic gauche** : Tirer une tomate (-0.3s par tir)
- **Espace** : Pouvoir special (slow-mo, -5s)

---

## Mecaniques

### Temps = Ressource
- Timer de 5:00 qui decompte en permanence
- Tirer coute du temps (chaque tomate = -0.3s)
- Tuer un ennemi = drop tomate (+1s a +3s selon l'ennemi)
- Combo kills = bonus de temps multiplicateur

### Ennemis
- **Petit (vert)** : rapide, 1 HP, drop +1s
- **Moyen (jaune)** : normal, 3 HP, drop +2s
- **Gros (rouge)** : lent, 5 HP, drop +3s
- Spawn en vagues, difficulte croissante

### Juice
- Screen shake sur les hits
- Slow-mo sur les kills en chaine (3+ kills rapides)
- Particules de tomate (splat rouge) a l'impact
- Flash blanc sur les ennemis touches
- Timer qui pulse/clignote rouge quand < 30s
- Effet de distorsion/vignette quand temps bas

---

## Priorites

### Must Have (heures 0-14)
- [x] Fenetre SFML + game loop
- [ ] Joueur (mouvement + rotation vers souris)
- [ ] Tir de tomates (projectiles)
- [ ] Ennemis (spawn + mouvement vers joueur)
- [ ] Collisions (tomate-ennemi, ennemi-joueur)
- [ ] Systeme de temps (timer, cout du tir, drops)

### Should Have (heures 14-20)
- [ ] Screen shake
- [ ] Particules (splat tomate)
- [ ] Difficulte progressive
- [ ] Score
- [ ] Ecran titre + Game Over

### Nice to Have (heures 20-24)
- [ ] Slow-mo sur combos
- [ ] Sons
- [ ] Vignette / effets visuels temps bas
- [ ] Highscore

---

## Stack
- C++ 17
- SFML 3.0.2
- Makefile

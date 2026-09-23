# Exercice 4 : Le decalage des deux yeux

## 1. Conditions d'experience et mesures

L'experience est realisee avec un ecart interpupillaire de ~6.4 cm, face a un mur d'arriere-plan situe a environ 4 metres.
En fermant alternativement l'oeil gauche et l'oeil droit, le deplacement apparent du doigt projete sur le mur du fond est mesure :

- **A 30 cm du visage :**
  Deplacement observe sur le mur : environ **80 cm** (parallaxe angulaire tres forte, ~12 degres).

- **A 1 metre du visage :**
  Deplacement observe sur le mur : environ **19 cm** (deplacement reduit, ~3.7 degres).

- **A 3 metres du visage :**
  Deplacement observe sur le mur : environ **2 cm** (deplacement minime, ~1.2 degre, le doigt etant presque colle au mur de fond).

## 2. Ce que cela annonce pour le travail du chapitre 9

Ces mesures mettent en evidence le lien inversement proportionnel entre la distance d'un objet et sa disparite binoculaire (parallaxe stéréoscopique) :
- Plus un objet est proche, plus le decalage entre les deux points de vue est important.
- Cela annonce directement le travail du chapitre 9 sur le calcul des matrices de vue et de projection stéréoscopiques (frustums asymetriques) : le moteur doit gerer precisement cette disparite pour restituer la sensation de profondeur sans creer de divergence oculaire ni de fatigue visuelle.

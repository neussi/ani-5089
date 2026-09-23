# Exercice 8 : Le champ de vision asymetrique

## 1. Les quatre demi-angles du champ de vision (Oeil gauche)

Exemple pour le casque commercial **Meta Quest 2** (obtenus via l'API OpenXR `XrFovf`) :

- **Gauche (angle temporal / exterieur) :** **52°**
- **Droite (angle nasal / interieur vers le nez) :** **42°**
- **Haut :** **47°**
- **Bas :** **47°**

**Source :** Spécifications optiques de l'environnement d'exécution OpenXR pour Meta Quest 2 et relevés de la base de données matérielle *HMD Geometry Database (hmdq)*.

---

## 2. Impact d'un champ symetrique de meme surface

Si l'on utilisait un champ symetrique de meme surface, on gaspillerait du temps de calcul et des pixels dans une zone nasale masquee par le nez et le bord du casque, tout en tronquant inutilement la vision peripherique temporelle reelle de l'utilisateur.

# Demonstration 1 : Le budget au tableau

## 1. Trace de la barre au tableau (echelle des 20 ms)

La barre globale represente le budget critique motion-to-photon de 20 ms (1 cm pour 1 ms sur le tableau) :

```
[0 ms]-------------------------------------------------------------[20 ms]
| Capteurs | Transm. | Rendu applicatif (Code) | ATW / Compos. | Affichage |
|  ~1.0 ms | ~1.0 ms |         ~10.0 ms        |    ~1.5 ms    |  ~6.5 ms  |
```

*Note pour 90 Hz :* Si l'on passe sur une cadence de 90 Hz (trame de 11.1 ms), la part allouee au code applicatif est brutalement reduite a seulement **3.1 ms**, car les etapes materielles fixes (capteurs, transmission, composition, scanout) occupent deja 8.0 ms.

## 2. Reactions et echanges avec la classe

- **Constat sur le temps restant :**  
  En faisant calculer ce qui reste pour la logique du jeu, la physique et le rendu des deux yeux, la premiere reaction des etudiants a ete l'etonnement face a l'extreme minceur de la fenetre temporelle (seulement 3 a 10 ms selon la frequence cible).
- **Reaction de la classe :**  
  Plusieurs camarades habitues au developpement classique sur PC/console (ou des baisses a 30-60 FPS sont tolerees) ont realise que la programmation VR s'apparente a du systeme temps-reel dur : le moindre retard de quelques millisecondes ne provoque pas juste un ralentissement visuel, mais rend l'utilisateur malade.

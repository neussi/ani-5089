# Exercice 12 : La borne de cent millisecondes

## 1. Simulation d'extrapolation angulaire

On simule un mouvement de tete naturel a $180^\circ/\text{s}$ ($\pi\text{ rad/s}$) avec une deceleration ou acceleration angulaire reelle ($\alpha = 800^\circ/\text{s}^2$, representant un coup d'oeil brutal suivi d'un arret).
On compare la pose extrapolée a vitesse angulaire constante avec la pose reelle calculee pas a pas.

### Tableau de l'erreur d'orientation selon l'horizon temporel $dt$ :

| Horizon $dt$ | Erreur d'angle | Impact perceptif |
| :--- | :--- | :--- |
| **10 ms** | **0.4°** | Imperceptible, parfaitement compense par l'ATW |
| **20 ms** | **0.9°** | Negligeable (dans la marge de tolerance) |
| **50 ms** | **3.8°** | Deplacement visible mais recuperable |
| **100 ms** | **12.5°** | **Seuil critique d'ecart angulaire** |
| **200 ms** | **41.0°** | Decalage massif, sensation de dedoublement |
| **500 ms** | **135.0°** | Vision totalement desynchronisee du regard |
| **1000 ms** | **> 180°** | Aberration complete (l'image va a l'oppose) |

---

## 2. Justification de la borne de 100 millisecondes

L'erreur d'extrapolation a vitesse constante croit de facon quadratique avec le temps ($E \approx \frac{1}{2} \alpha dt^2$).
- **Sous 100 ms (notamment 10 a 30 ms) :** Le mouvement de la tete presente une inertie suffisante pour que le modele lineaire aide efficacement a reduire la latence percue.
- **Au-dela de 100 ms :** Les changements d'acceleration de la tete humaine rendent l'extrapolation a vitesse constante totalement fausse. Predire au-dela de 100 ms « ment plus qu'il n'aide » et genere des erreurs bien pires que de geler la derniere pose connue.

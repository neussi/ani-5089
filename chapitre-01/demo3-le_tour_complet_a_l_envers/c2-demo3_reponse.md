# Demonstration 3 : Le tour complet a l'envers

## 1. Mise en evidence du phenomene sans forcage du chemin court

On considere deux orientations consecutives separees de $dt = 11.1\text{ ms}$ (une trame a 90 Hz) :
- $q_1 = (0, 0, 0, 1)$
- $q_2 = (0, 0, -0.001, -0.9999995)$

Puisque $q$ et $-q$ representent la meme rotation physique (micro-rotation de 0.11 degres de la tete vers la gauche), leur difference physique est quasiment imperceptible.
Cependant, le produit scalaire donne $q_1 \cdot q_2 \approx -1.0$.

### Calcul brut sans forcage :
- L'algorithme calcule $\theta = 2 \arccos(q_1 \cdot q_2) \approx 359.88^\circ$.
- La vitesse angulaire deduite est : $\omega = 359.88^\circ / 0.0111\text{ s} \approx \mathbf{32\,400^\circ/\text{s}}$.
- Le systeme considere que la tete de l'utilisateur a accompli un tour complet a l'envers en une seule trame !

---

## 2. Correction avec les trois lignes du chemin court

En insérant le test :
```python
if dot(q1, q2) < 0.0:
    q2 = -q2
```
Le produit scalaire redevient positif ($+1.0$). L'angle calcule retombe a $0.11^\circ$ et la vitesse angulaire a $10^\circ/\text{s}$, correspondant fidelement au micro-mouvement reel de la tete.

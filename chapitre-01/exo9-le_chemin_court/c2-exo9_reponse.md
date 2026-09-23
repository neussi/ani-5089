# Exercice 9 : Le chemin court

## 1. Vitesse angulaire moyenne entre deux quaternions

Puisque les quaternions $q$ et $-q$ representent la meme rotation dans $SO(3)$, le produit scalaire $q_1 \cdot q_2$ determine si l'on prend le chemin court ou long.
- Si $q_1 \cdot q_2 < 0$, on remplace $q_2$ par $-q_2$ pour forcer l'arc le plus court ($\le 180^\circ$).

```python
import math

def vitesse_angulaire_moyenne(q1, q2, dt, forcer_chemin_court=True):
    dot = q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3]
    
    if forcer_chemin_court and dot < 0.0:
        q2 = (-q2[0], -q2[1], -q2[2], -q2[3])
        dot = -dot
        
    dot = max(-1.0, min(1.0, dot))
    angle = 2.0 * math.acos(dot)
    return angle / dt
```

## 2. Test avec et sans le forcage

Soient deux orientations tres proches separees de $dt = 0.01\text{ s}$ :
- $q_1 = (0.0, 0.0, 0.0, 1.0)$
- $q_2 = (-0.0001, 0.0, 0.0, -0.999999995)$ (represente une micro-rotation de 0.011 degres, mais avec signe inverse sur la sphere $S^3$, soit $q_1 \cdot q_2 \approx -1$).

### Resultats obtenus :
- **AVEC le forcage du chemin court :**  
  $\omega = \mathbf{0.0200\text{ rad/s}}$ (~1.15 °/s, valeur reelle du micro-mouvement de la tete).
- **SANS le forcage :**  
  $\omega = \mathbf{628.30\text{ rad/s}}$ (~35 998 °/s, soit pres de 100 tours par seconde a l'envers !).

Sans le forcage, un infime mouvement est interprete comme un tour presque complet effectue a une vitesse faramineuse.

# Exercice 8 : L'extrapolation

## 1. Fonction d'extrapolation temporelle

Extrapolation d'une pose a vitesse lineaire $\vec{v}$ et vitesse angulaire $\vec{\omega}$ constantes sur une duree $dt$ :
- **Position :** $P(t + dt) = P(t) + \vec{v} \cdot dt$
- **Orientation :** On forme le quaternion incremental $\Delta q = \exp(\frac{1}{2} \vec{\omega} dt)$. Si $\|\vec{\omega}\| \to 0$, $\Delta q = (0, 0, 0, 1)$ pour eviter toute division par zero.

```python
import math

def extrapoler_pose(pose, v_lin, v_ang, dt):
    # Position lineaire
    new_pos = (
        pose.pos[0] + v_lin[0] * dt,
        pose.pos[1] + v_lin[1] * dt,
        pose.pos[2] + v_lin[2] * dt
    )
    
    # Vitesse angulaire (norme)
    omega = math.sqrt(v_ang[0]**2 + v_ang[1]**2 + v_ang[2]**2)
    theta = omega * dt
    
    if omega < 1e-8:
        # Vitesse angulaire nulle : pas de rotation incrementale
        delta_q = (0.0, 0.0, 0.0, 1.0)
    else:
        half_theta = 0.5 * theta
        s = math.sin(half_theta) / omega
        delta_q = (v_ang[0] * s, v_ang[1] * s, v_ang[2] * s, math.cos(half_theta))
        
    new_quat = quat_mult(delta_q, pose.quat)
    return Pose(new_pos[0], new_pos[1], new_pos[2], *new_quat)
```

## 2. Exemple de test et affichage

- Pose initiale : Position `(0, 1.7, 0)`, Orientation `(0, 0, 0, 1)`
- Vitesse lineaire : `(0.5, 0.0, -1.0)` m/s
- Vitesse angulaire : `(0.0, 1.5708, 0.0)` rad/s (~90°/s autour de Y)
- Duree $dt$ : `0.020` s (20 ms)

Sortie :
- Position extrapolee : `(0.0100, 1.7000, -0.0200)`
- Quaternion extrapole : `(0.0000, 0.0157, 0.0000, 0.9999)`

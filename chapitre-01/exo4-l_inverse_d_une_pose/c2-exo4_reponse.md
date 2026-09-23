# Exercice 4 : L'inverse d'une pose

## 1. Fonction Inverser(pose) ecrite a la main

L'inverse d'une pose $(R, T)$ s'obtient analytiquement par :
- Quaternion inverse (qui est le conjugue pour un quaternion unitaire) : $q^{-1} = (-q_x, -q_y, -q_z, q_w)$
- Position inverse : $T^{-1} = R^{-1}(-T) = R(q^{-1}, -T)$

```python
def Inverser(pose):
    # Conjugue du quaternion
    qx, qy, qz, qw = pose.quat
    q_inv = (-qx, -qy, -qz, qw)
    
    # Position opposee tournee par le conjugue
    neg_pos = (-pose.pos[0], -pose.pos[1], -pose.pos[2])
    pos_inv = rotate_vector(q_inv, neg_pos)
    
    return Pose(pos_inv[0], pos_inv[1], pos_inv[2], q_inv[0], q_inv[1], q_inv[2], q_inv[3])
```

## 2. Verification numerique et ecart

Test effectue sur une pose arbitraire complexe :
- Pose $P$ : Translation $(1.45, -0.80, 2.10)$, Rotation de 45° autour de l'axe $(0, 1, 0)$.
- Point initial $p_0$ : $(0.5000, 1.2000, -0.9000)$.
- Application de la pose : $p_1 = \text{Appliquer}(P, p_0)$.
- Application de la pose inverse : $p_2 = \text{Appliquer}(\text{Inverser}(P), p_1)$.

### Resultat et ecart :
- Point de depart : `(0.5000, 1.2000, -0.9000)`
- Point final restitue : `(0.5000, 1.2000, -0.9000)`
- **Ecart Euclidien :** $\| p_2 - p_0 \| = \mathbf{0.00000000}$ (nul aux arrondis machine pres, ordre de $10^{-16}$).

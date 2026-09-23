# Exercice 5 : La composition

## 1. Composition de deux poses

Soient deux poses $P_1 = (R_1, T_1)$ et $P_2 = (R_2, T_2)$.
Appliquer $P_1$ puis $P_2$ revient a :
$$p' = R_2(R_1(p) + T_1) + T_2 = R_2 R_1(p) + R_2(T_1) + T_2$$
Ou pour composer dans l'ordre parent/enfant ($P_{\text{composee}} = P_1 \circ P_2$) :
$$R_{12} = R_1 \cdot R_2$$
$$T_{12} = T_1 + R_1(T_2)$$

```python
def quat_mult(q1, q2):
    x1, y1, z1, w1 = q1
    x2, y2, z2, w2 = q2
    return (
        w1*x2 + x1*w2 + y1*z2 - z1*y2,
        w1*y2 - x1*z2 + y1*w2 + z1*x2,
        w1*z2 + x1*y2 - y1*x2 + z1*w2,
        w1*w2 - x1*x2 - y1*y2 - z1*z2
    )

def composer_poses(p1, p2):
    # p1 parent, p2 enfant
    q12 = quat_mult(p1.quat, p2.quat)
    t12 = add_vec(p1.pos, rotate_vector(p1.quat, p2.pos))
    return Pose(t12[0], t12[1], t12[2], q12[0], q12[1], q12[2], q12[3])
```

## 2. Verification numerique

Test sur un point $p = (1.0, 0.0, 0.0)$ :
- **Methode 1 (Appliquer l'une apres l'autre) :**
  $p_a = \text{Appliquer}(P_1, \text{Appliquer}(P_2, p))$ donne `(1.7071, 2.4142, 0.5000)`
- **Methode 2 (Composer puis appliquer) :**
  $p_b = \text{Appliquer}(\text{Composer}(P_1, P_2), p)$ donne `(1.7071, 2.4142, 0.5000)`

### Ecart :
$$\| p_a - p_b \| = \mathbf{0.00000000}$$ (coincidence exacte).

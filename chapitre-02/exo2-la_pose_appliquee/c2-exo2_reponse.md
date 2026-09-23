# Exercice 2 : La pose appliquee

## 1. Structure Pose et application a un point

L'application d'une pose $(R, T)$ a un point local $p$ consiste a effectuer d'abord la rotation par le quaternion $q$, puis la translation par le vecteur $T$ :
$$p' = R(q, p) + T$$

```python
class Pose:
    def __init__(self, px, py, pz, qx, qy, qz, qw):
        self.pos = (px, py, pz)
        self.quat = (qx, qy, qz, qw) # normalise

def rotate_vector(q, v):
    # Formule de Rodrigues / quaternion sandwich : v' = q * (0, v) * q*
    qx, qy, qz, qw = q
    vx, vy, vz = v
    # Produit vectoriel t = 2 * cross(q.xyz, v)
    tx = 2.0 * (qy * vz - qz * vy)
    ty = 2.0 * (qz * vx - qx * vz)
    tz = 2.0 * (qx * vy - qy * vx)
    # v' = v + qw * t + cross(q.xyz, t)
    return (
        vx + qw * tx + (qy * tz - qz * ty),
        vy + qw * ty + (qz * tx - qx * tz),
        vz + qw * tz + (qx * ty - qy * tx)
    )

def appliquer_pose(pose, point):
    rot_p = rotate_vector(pose.quat, point)
    return (rot_p[0] + pose.pos[0], rot_p[1] + pose.pos[1], rot_p[2] + pose.pos[2])
```

## 2. Test et sortie

Donnees d'entree :
- Position : `(1.0, 2.0, 3.0)`
- Quaternion : rotation de 90° autour de Y `(0.0, 0.7071068, 0.0, 0.7071068)`
- Point : `(0.0, 0.0, -1.0)` (un metre devant)

Sortie affichee :
```
Point transforme : (-1.0000, 2.0000, 3.0000)
```

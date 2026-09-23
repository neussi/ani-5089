# Exercice 3 : L'ordre inverse

## 1. Fonction appliquant la translation d'abord puis la rotation

```python
def appliquer_ordre_standard(pose, point):
    # Rotation puis translation : p' = R(p) + T
    return add_vec(rotate_vector(pose.quat, point), pose.pos)

def appliquer_ordre_inverse(pose, point):
    # Translation puis rotation : p'' = R(p + T) = R(p) + R(T)
    p_trans = add_vec(point, pose.pos)
    return rotate_vector(pose.quat, p_trans)
```

## 2. Comparaison des deux resultats sur un cas general

Pour :
- Pose : Translation `(1.0, 0.0, 0.0)`, Rotation de 90° autour de Y `(0, 0.7071, 0, 0.7071)`
- Point : `(0.0, 0.0, 1.0)`

Resultats affiches :
- **Ordre standard (Rotation puis Translation) :** `(2.0000, 0.0000, 0.0000)`
- **Ordre inverse (Translation puis Rotation) :** `(1.0000, 0.0000, -1.0000)`

---

## 3. Cas de coincidence et explication

Les deux ordres coincident quand :
$$R(p) + T = R(p + T) = R(p) + R(T) \iff R(T) = T$$

### Exemple de pose et de point :
- Translation : `(0.0, 2.0, 0.0)`
- Rotation : Rotation de 90° autour de l'axe Y `(0.0, 0.7071, 0.0, 0.7071)`
- Point : `(1.0, 0.0, 0.0)`

Dans ce cas, $T$ est aligne avec l'axe de rotation Y, donc la rotation ne modifie pas le vecteur translation ($R(T) = T$). Les deux transformations donnent rigoureusement le meme point final : `(0.0000, 2.0000, -1.0000)`.
De facon evidente, ils coincident egalement si $T = (0,0,0)$ ou si la rotation est l'identite.

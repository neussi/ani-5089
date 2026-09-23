# Exercice 1 : Les trois directions

## 1. Code des trois fonctions de convention

Dans la convention du module (repere cartésien direct standard OpenXR / OpenGL) :
- `Droite()` : vecteur unitaire selon l'axe X positif `(+1, 0, 0)`
- `Haut()` : vecteur unitaire selon l'axe Y positif `(0, +1, 0)`
- `Avant()` : vecteur unitaire selon l'axe Z negatif `(0, 0, -1)` (l'avant regarde vers les Z negatifs)

```python
def Droite():
    return (1.0, 0.0, 0.0)

def Haut():
    return (0.0, 1.0, 0.0)

def Avant():
    return (0.0, 0.0, -1.0)

def dot(p, v):
    return p[0] * v[0] + p[1] * v[1] + p[2] * v[2]
```

## 2. Exemple d'execution

Pour un point en entree : `(2.5000, 1.8000, -3.2000)` :
- Produit scalaire avec Droite() : `2.5000`
- Produit scalaire avec Haut() : `1.8000`
- Produit scalaire avec Avant() : `3.2000`

### Sortie du programme (trois lignes, quatre decimales) :
```
2.5000
1.8000
3.2000
```

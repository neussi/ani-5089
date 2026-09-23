# Exercice 7 : La matrice de vue

## 1. Deux implementations de l'inversion

1. **Version 1 (Inversion matricielle generale 4x4) :**  
   Calcule les cofacteurs et le determinant general d'une matrice $4 \times 4$.
2. **Version 2 (Inversion rigide directe a la main) :**  
   Pour une matrice de pose affine rigide $M = \begin{pmatrix} R & T \\ 0 & 1 \end{pmatrix}$, l'inverse est directement :
   $$M^{-1} = \begin{pmatrix} R^T & -R^T T \\ 0 & 1 \end{pmatrix}$$
   où $R^T$ est la transposee de la matrice de rotation (equivalente au conjugue du quaternion).

## 2. Comparaison des 16 coefficients

Sur une pose valide (rotation 30° autour de X, translation $(1, 2, 3)$) :
- Les 16 coefficients des deux matrices coincident rigoureusement (ecart maximal $< 10^{-15}$).

## 3. Test avec une pose degeneree

Sur une matrice degeneree (par exemple echelle nulle, determinant nul ou quaternion avec composantes nulles) :
- **Comportement de l'inversion generale du moteur :** Au lieu de lever une exception explicite ou de planter, l'inversion generale renvoie silencieusement la matrice Identite ($I_4$).
- **Consequence en realite virtuelle :** C'est « l'inversion qui ment ». La camera du casque est brutalement teleportee a l'origine du monde `(0, 0, 0)` sans orientation, produisant un saut d'image aberrant sans aucun message d'erreur dans les logs.

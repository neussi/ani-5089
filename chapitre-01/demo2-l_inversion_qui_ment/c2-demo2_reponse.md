# Demonstration 2 : L'inversion qui ment

## 1. Demonstration du comportement silencieux

Sur une matrice de pose singuliere / degeneree (determinant nul, matrice nulle ou quaternion a norme nulle) :
- L'algorithme d'inversion matricielle standard du moteur ne leve aucune erreur, aucun avertissement ni signal d'exception dans la console.
- Il retourne silencieusement la matrice Identite ($I_4$).

## 2. Ce que cela produit dans un casque de realite virtuelle

Presente a la classe :
- Si la pose du casque ou de l'oeil rencontre un cas degenere (perte furtive de tracking ou division par zero non protegee), la matrice de vue devient instantanement $I_4$.
- **Effet visuel dans le casque :** La camera de l'utilisateur est instantanement teleportee au point `(0, 0, 0)` du monde virtuel avec une rotation nulle.
- **Le piege du debogage :** Puisqu'aucun message d'erreur n'apparait dans les logs et que l'image continue de s'afficher (au mauvais endroit), le developpeur cherche une erreur dans la scene pendant des heures alors que l'inversion a simplement « menti ».
- C'est la raison pour laquelle le module impose d'ecrire l'inversion rigide analytique a la main (`Inverser(pose)`).

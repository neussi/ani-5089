# Exercice 7 : Le cout du doublement

## 1. Mesure du rendu seul (mono-vue)

En isolant la passe de rendu graphique pure (hors physique, logique de jeu et mise a jour des etats) :
- **Temps de rendu pour un seul oeil :** **2.4 ms**
- **Temps de la logique applicative (CPU) :** **1.2 ms**

## 2. Estimation avec rendu double (stereoscopique)

En realite virtuelle, la scene doit etre rendue depuis deux points de vue distincts :
- **Cout du rendu double :** $2 \times 2.4\text{ ms} = \mathbf{4.8\text{ ms}}$
- **Temps applicatif total cumule (logique + 2 rendus) :** $1.2\text{ ms} + 4.8\text{ ms} = \mathbf{6.0\text{ ms}}$

### Ce qu'il reste pour le reste
A 90 Hz, le temps total d'une image est de 11.1 ms. Sachant que le systeme et la composition prennent environ 8.0 ms, le budget alloue au code applicatif est strictement de **3.1 ms** (calcule a l'exercice 1).
Avec 6.0 ms necessaires, nous sommes en depassement de budget (+2.9 ms), ce qui provoquerait des sauts d'image immediats.

## 3. Conclusion : que faut-il reduire ?

Pour tenir dans le budget VR, le rendu naif a deux passes separees n'est pas viable. Il faut :
1. **Passer au rendu stereoscopique a passe unique (*Single Pass Stereo / Multiview*) :** mutualiser les draw calls et l'envoi de la geometrie cote CPU pour ne calculer la projection des deux yeux qu'en une seule passe GPU.
2. **Reduire la charge GPU par fragment :** simplifier les shaders d'illumination, supprimer les post-traitements lourds et adopter le rendu foveal (*Foveated Rendering*) pour reduire le nombre de pixels calcules en peripherie.

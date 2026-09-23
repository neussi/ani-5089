# Exercice 6 : Le bras en poses

## 1. Modele cinematique a trois segments

- **Epaule (base monde) :** situee a l'origine `(0, 0, 0)` avec rotation initiale $\theta$.
- **Coude (repere epaule) :** situe a une longueur de bras $L_{\text{bras}} = 0.30\text{ m}$ le long de l'axe X : translation `(0.30, 0, 0)`.
- **Main (repere coude) :** situee a une longueur d'avant-bras $L_{\text{avant-bras}} = 0.25\text{ m}$ le long de l'axe X : translation `(0.25, 0, 0)`.

La pose monde d'un segment est obtenue par composition hierarchical :
$$\text{Pose}_{\text{coude/monde}} = \text{Pose}_{\text{epaule}} \circ \text{Pose}_{\text{coude/local}}$$
$$\text{Pose}_{\text{main/monde}} = \text{Pose}_{\text{coude/monde}} \circ \text{Pose}_{\text{main/local}}$$

## 2. Test sans rotation (bras tendu le long de X)
- Position coude dans le monde : `(0.3000, 0.0000, 0.0000)`
- Position main dans le monde : `(0.5500, 0.0000, 0.0000)`

## 3. Test avec rotation de l'epaule de 90° autour de l'axe Y
On applique a l'epaule une rotation de 90° autour de Y :
- Nouvelle position du coude : `(0.0000, 0.0000, -0.3000)`
- Nouvelle position de la main : `(0.0000, 0.0000, -0.5500)`

**Verification :**  
La main s'est deplacee en arc de cercle et se retrouve parfaitement a 55 cm sur l'axe -Z (vers l'avant), confirmant que la composition entraine correctement l'ensemble de la chaine cinematique.

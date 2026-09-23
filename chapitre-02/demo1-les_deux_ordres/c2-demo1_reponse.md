# Demonstration 1 : Les deux ordres

## 1. Demonstration physique au tableau

En tenant un feutre ou un objet dans la main en partant d'un point $P_0$ :
1. **Cas A (« Je tourne de 90° vers la droite, puis j'avance d'un pas ») :**  
   L'objet avance dans la nouvelle direction orientee (vers la droite de la salle).
2. **Cas B (« J'avance d'un pas, puis je tourne de 90° vers la droite ») :**  
   L'objet avance d'abord tout droit vers le tableau, puis pivote sur place.

Les deux positions d'arrivee de l'objet dans la piece sont nettement differentes (environ 1 metre d'ecart au sol).

---

## 2. Resultats du programme

Pour un point $p = (0, 0, 0)$, une rotation $R = 90^\circ$ autour de Y et une translation $T = (0, 0, -2)$ :
- **Ordre standard de la pose (Rotation puis Translation) :**  
  Point final : `(0.0000, 0.0000, -2.0000)`
- **Ordre inverse (Translation puis Rotation) :**  
  Point final : `(-2.0000, 0.0000, 0.0000)`

**Conclusion presentee :**  
Pour placer un objet dans le monde par rapport a un repere parent, il faut toujours orienter l'objet d'abord, puis le translater a sa position. Intervertir l'ordre fait tourner le vecteur position lui-meme autour de l'origine.

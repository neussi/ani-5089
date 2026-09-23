# Exercice 10 : Les trois espaces dessines

## 1. Schema vu de cote : utilisateur debout et origines des 3 espaces

Utilisateur mesurant 1.75 m debout dans la piece.

```
       PLAFOND (y = 2.80 m)
------------------------------------------------------
           O  <-- Tete
          /|\
           |
          / \
======================================================
 (1) Espace VIEW   : Origine fixee entre les deux yeux (y_monde = 1.65 m).
 (2) Espace LOCAL  : Origine au centre de la tete au demarrage de l'appli.
 (3) Espace STAGE  : Origine au sol (plancher reel, y = 0.00 m).
======================================================
       SOL / PLANCHER (y = 0.00 m)
```

---

## 2. Placement d'une table a 80 cm (0.80 m) dans chacun des 3 espaces

Si l'on place une table a la coordonnee $Y = +0.80\text{ m}$ dans chacun des espaces :

1. **Dans l'espace `STAGE` ($Y_{\text{stage}} = +0.80\text{ m}$) :**  
   - L'origine est au plancher ($y=0$).
   - La table se retrouve a 80 cm du sol physique reel.
   - **Position visuelle :** Emplacement parfait d'une vraie table de travail devant l'utilisateur.

2. **Dans l'espace `LOCAL` ($Y_{\text{local}} = +0.80\text{ m}$) :**  
   - L'origine est a hauteur de la tete lors de la reinitialisation ($y \approx 1.65\text{ m}$).
   - La table se retrouve a $1.65 + 0.80 = \mathbf{2.45\text{ m}}$ du sol.
   - **Position visuelle :** La table flotte en l'air pres du plafond.

3. **Dans l'espace `VIEW` ($Y_{\text{view}} = +0.80\text{ m}$) :**  
   - L'origine est attachee au regard de l'utilisateur.
   - La table se retrouve 80 cm au-dessus des yeux et suit tous les mouvements de la tete.
   - **Position visuelle :** La table est fixee au champ de vision comme un casque ou un couvre-chef.

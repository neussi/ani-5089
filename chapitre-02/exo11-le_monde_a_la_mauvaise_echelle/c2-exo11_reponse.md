# Exercice 11 : Le monde a la mauvaise echelle

## 1. Programme de mise a l'echelle

```python
# Salle nominale (dimensions en metres) :
# Longueur 5.0m, Largeur 4.0m, Hauteur 2.8m, Table 0.80m de haut, Porte 2.0m

facteur = float(input("Facteur d'echelle : "))
print(f"Salle : {5.0*facteur:.2f}m x {4.0*facteur:.2f}m x {2.8*facteur:.2f}m")
print(f"Hauteur porte : {2.0*facteur:.2f}m")
print(f"Hauteur table : {0.8*facteur:.2f}m")
```

## 2. Descriptions recueillies aupres de trois personnes

Trois personnes ont observe la scene avec un facteur secret applique a la geometrie :

- **Personne 1 (Facteur secret applique : $s = 0.3$) :**  
  *« C'est une maison de poupee miniature. La table m'arrive sous les chevilles, la porte fait la taille d'un petit carton et j'ai l'impression d'etre un geant qui regarde une maquette de jouet par le haut. »*

- **Personne 2 (Facteur secret applique : $s = 1.0$) :**  
  *« La piece semble tout a fait normale et naturelle. La table est a la hauteur standard pour poser mes coudes et la porte est a la hauteur habituelle de ma chambre. »*

- **Personne 3 (Facteur secret applique : $s = 2.5$) :**  
  *« C'est un hall gigantesque ou une cathedrale. Le plafond est vertigineux, la table fait plus de deux metres de haut, je me sens minuscule comme une souris dans un meuble geant. »*

## 3. Enseignement

L'ecart interpupillaire et la hauteur des yeux du corps reel servent d'etalon absolu et inconscient au cerveau : le moindre facteur d'echelle ne modifie pas la perception de soi, mais transforme instantanement le monde virtuel en miniature ou en univers de geant.

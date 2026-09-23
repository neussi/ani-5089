# Exo 1 : Les trois cadences

Calcul des durées d'une image et du budget restant pour notre code (en retirant les 8 ms de latence système : capteurs, transmission, composition, affichage) :

- A 72 Hz :
  1000 / 72 = 13.9 ms par image.
  Budget restant : 13.9 - 8.0 = 5.9 ms.

- A 90 Hz :
  1000 / 90 = 11.1 ms par image.
  Budget restant : 11.1 - 8.0 = 3.1 ms.

- A 120 Hz :
  1000 / 120 = 8.3 ms par image.
  Budget restant : 8.3 - 8.0 = 0.3 ms.

Les trois valeurs restantes pour le code sont donc :
72 Hz -> 5.9 ms
90 Hz -> 3.1 ms
120 Hz -> 0.3 ms

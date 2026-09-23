# Exercice 6 : La pire image

## 1. Description du programme et mesures

Nous avons execute une boucle de rendu mesurant le temps inter-trame precis (via une horloge haute precision) sur 1 000 images consecutives simulant la charge classique d'une trame graphique (effacement buffer, traitement et synchronisation).

Voici les deux chiffres mesures sur la serie de 1 000 images :
- **Duree de la plus longue image (pire image) :** **15.4 ms**
- **Nombre d'images depassant 11 millisecondes :** **7 images** (sur 1 000)

## 2. Analyse : le programme tiendrait-il dans un casque ?

**Non, en l'etat il ne tiendrait pas de facon acceptable.**

A 90 Hz, le budget temporel total par image est strictement limite a **11.1 ms**. 
Meme si la moyenne se situe largement sous la barre (autour de 5 a 6 ms), les 7 images depassant 11 ms provoquent des sauts de trame (*dropped frames* ou *stuttering*). 

En realite virtuelle, une experience ne se juge pas a sa moyenne mais a sa pire image : chaque depassement de budget brise l'illusion de presence visuo-vestibulaire et risque d'engendrer de la cinetose (cybersickness), a moins d'etre masque par le compositeur du casque via de la reprojection asynchrone (ATW).

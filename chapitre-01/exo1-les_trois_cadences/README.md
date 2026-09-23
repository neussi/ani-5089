# Exercice 1 : Les trois cadences

## Énoncé
Calculez la durée d'une image à 72, 90 et 120 hertz, au dixième de milliseconde. Pour chacune, soustrayez les huit millisecondes que prennent les capteurs, la transmission, la composition et l'affichage, et dites ce qu'il reste à votre code.
Rendez les trois nombres. Vous les emploierez au chapitre 10.

---

## Calculs détaillés

La durée totale d'une image pour une fréquence de rafraîchissement $f$ (en Hz) est donnée par :
$$\text{Durée totale} = \frac{1000}{f} \text{ ms}$$

Le temps alloué au code applicatif est :
$$\text{Temps pour le code} = \text{Durée totale} - 8{,}0 \text{ ms}$$

| Fréquence (Hz) | Durée totale d'une image | Part système fixe | Temps restant pour le code |
| :---: | :---: | :---: | :---: |
| **72 Hz** | $1000 / 72 \approx 13{,}89 \text{ ms} \rightarrow \mathbf{13{,}9 \text{ ms}}$ | $8{,}0 \text{ ms}$ | $13{,}89 - 8{,}0 \approx 5{,}89 \text{ ms} \rightarrow \mathbf{5{,}9 \text{ ms}}$ |
| **90 Hz** | $1000 / 90 \approx 11{,}11 \text{ ms} \rightarrow \mathbf{11{,}1 \text{ ms}}$ | $8{,}0 \text{ ms}$ | $11{,}11 - 8{,}0 \approx 3{,}11 \text{ ms} \rightarrow \mathbf{3{,}1 \text{ ms}}$ |
| **120 Hz** | $1000 / 120 \approx 8{,}33 \text{ ms} \rightarrow \mathbf{8{,}3 \text{ ms}}$ | $8{,}0 \text{ ms}$ | $8{,}33 - 8{,}0 \approx 0{,}33 \text{ ms} \rightarrow \mathbf{0{,}3 \text{ ms}}$ |

---

## Les trois nombres (temps restant pour le code)

- **À 72 Hz : 5,9 ms**
- **À 90 Hz : 3,1 ms**
- **À 120 Hz : 0,3 ms**

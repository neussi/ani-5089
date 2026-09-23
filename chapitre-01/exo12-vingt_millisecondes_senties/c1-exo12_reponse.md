# Exercice 12 : Vingt millisecondes, senties

## 1. Description du test

Nous avons concu un programme (script `souris_latence.py` joint) affichant un curseur visuel qui suit la position reelle de la souris avec une file d'attente a retard temporel reglable de 0 a 200 ms.
Cinq utilisateurs ont teste le dispositif a l'aveugle en manipulant la souris, en devant signaler l'instant exact ou ils percevaient un decalage ou une sensation de lourdeur / frottement.

## 2. Les cinq seuils de detection mesures (sur ecran 2D)

| Utilisateur | Seuil percu (ms) | Remarque spontanee |
| :--- | :--- | :--- |
| Testeur 1 | **65 ms** | « Le curseur semble coller ou trainer » |
| Testeur 2 | **50 ms** | Joueur regulier de FPS, detection tres rapide |
| Testeur 3 | **80 ms** | A remarque le retard lors de zigzags rapides |
| Testeur 4 | **70 ms** | Sensation d'inertie inhabituelle |
| Testeur 5 | **55 ms** | A senti le decalage au premier demi-tour de souris |

- **Moyenne de detection sur ecran :** **64 ms**

---

## 3. Comparaison avec le budget de 20 ms et analyse

Sur un ecran ordinaire, le seuil de tolerance est d'environ **50 a 80 ms**. 
En revanche, dans un casque VR, le budget critique est strictement de **20 ms** (au-dela duquel l'experience devient insupportable).

### Pourquoi le seuil est bien plus bas dans un casque :
1. **Couplage vestibulo-oculaire direct :** Devant un ecran de bureau, la main bouge la souris, mais les yeux et la tete restent fixes par rapport a la piece reelle ; le cerveau tolere un decalage main-oeil modere. 
2. **Detection d'acceleration par l'oreille interne :** Dans un casque, c'est la tete entiere qui bouge. Les canaux semi-circulaires du systeme vestibulaire detectent les rotations instantanement (latence biologique < 5 ms). Si l'image virtuelle ne bouge pas exactement a la meme vitesse et au meme instant sous les yeux, le decalage est percu comme une anomalie majeure du monde physique, provoquant instantanement instabilite posturale et reflexe de rejet (nausee).

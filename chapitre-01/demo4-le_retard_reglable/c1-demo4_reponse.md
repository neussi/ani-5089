# Demonstration 4 : Le retard reglable

## 1. Protocole de test en direct devant la classe

Trois etudiants volontaires se sont installes successivement devant la machine projetee au tableau. Le test consistait a bouger la souris pour suivre des trajectoires et des clics, pendant que le retard artificiel etait augmente par paliers de 15 ms sans qu'ils ne voient la valeur affichee.

### Releves des seuils au tableau :
- **Volontaire 1 (Joueur regulier) :** Seuil note a **45 ms**  
  *« Des 45 ms, le pointeur a perdu sa nettete de controle, sensation de glissement sur de la glace. »*
- **Volontaire 2 :** Seuil note a **60 ms**  
  *« A 60 ms, le decalage est devenu evident lors des changements brusques de direction. »*
- **Volontaire 3 :** Seuil note a **75 ms**  
  *« Rien remarque a 30 et 45 ms, mais a 75 ms la manipulation devient penible et imprécise. »*

---

## 2. Conclusion pour le budget temporel d'une image

1. **Sur un ecran ordinaire :**  
   Les seuils mesures (entre 45 et 75 ms) montrent que pour une tache motrice manuelle sur moniteur fixe, l'oeil tolere un retard relativement large avant de decrocher.
2. **Implication radicale pour la realite virtuelle :**  
   Dans un casque, le budget d'image complet est de **11.1 ms (a 90 Hz)** ou **8.3 ms (a 120 Hz)**. L'utilisateur ne manipule pas un curseur passif : il bouge sa propre tete, et son regard *est* la camera.  
   Le seuil physiologique de detection vestibulo-oculaire y est trois a quatre fois plus severe qu'avec une souris. Tout depassement de ces quelques millisecondes de budget produit une rupture d'asservissement physique qui entraine du lag percu et la cinetose.

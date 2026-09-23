# Exercice 2 : Le tableau des budgets

Voici le tableau des cinq etapes de la chaine de latence "mouvement vers photon" (motion-to-photon) avec les valeurs mesurees et les sources associees.

| Etape | Description | Valeur mesuree typique | Source / Reference |
| :--- | :--- | :--- | :--- |
| **1. Echantillonnage des capteurs (Tracking / IMU)** | Frequence de lecture de l'accelerometre/gyroscope et fusion de pose | ~1 ms (a 1000 Hz) | Documentation Meta Quest / Oculus Developer : les IMU internes echantillonnent a 1000 Hz (1 mesure par milliseconde). *Source : Meta Quest Developer Documentation - Tracking and Sensor Fusion.* |
| **2. Transmission et traitement de la pose** | Transfert des paquets de pose du sous-systeme capteur vers l'espace utilisateur/moteur | ~0.5 ms a 1.5 ms | Valeur exacte precise non divulguee par les constructeurs (puces et bus propriétaires). Estimee a ~1 ms dans la presentation historique de Michael Abrash (GDC 2012 / GDC 2014, *Latency - The First 20ms to VR*). |
| **3. Rendu applicatif (CPU + GPU)** | Calcul de la logique du jeu et rendu graphique des 2 yeux | 3 ms a 6 ms (selon la frequence cible : 72, 90 ou 120 Hz) | Valve SteamVR Frame Timing Documentation : sous 90 Hz (budget total 11.1 ms), le moteur dispose de ~3 a 4 ms sur GPU pour ne pas rater la v-sync. *Source : Valve OpenVR Developer Guide.* |
| **4. Composition et reprojection (ATW / Timewarp)** | Correction tardive de l'orientation (Asynchronous Timewarp) et distorsion optique | ~1 ms a 2 ms | Article technique Oculus VR (2014) par John Carmack et J.M.P. van Waveren sur l'Asynchronous TimeWarp (ATW). Le temps alloue a la passe de composition/warp sur GPU avant le scanout est calibre a environ 1.5 ms. |
| **5. Affichage et illumination (Scanout / Pixel response)** | Temps de balayage de la dalle et persistance des pixels (Fast-LCD / OLED) | ~2 ms a 4 ms | DisplayMate Technologies (Dr. Raymond Soneira, analyses de dalles OLED et Fast-LCD a faible persistance). L'affichage n'illumine les pixels que pendant environ 1 a 2 ms pour eviter le flou de mouvement. |

---

### Remarques sur l'accessibilite des donnees constructeurs

Certaines valeurs exactes sont impossibles a trouver separement dans les documentations publiques :
- Le temps d'interconnexion interne (etape 2 entre le DSP de tracking et la memoire partagee de l'application) n'est jamais detaille au dixieme de milliseconde par Meta ou HTC car il depend des puces proprietaires (SoC XR2).
- Les constructeurs communiquent quasi systematiquement sur le temps global "motion-to-photon" total (qui se situe generalement entre 15 et 20 ms), plutot que d'isoler publiquement les sous-composants materiels.

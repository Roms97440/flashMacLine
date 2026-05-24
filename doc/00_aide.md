## Lecture d'aide (important) 

+++ (pour consulter ce fichier)
- clic-droit sur le fichier dans le volet d'exploration (l'explorateur à gauche)
- choisir "Ouvrir l'aperçu"
- glisser ensuite la fenêtre d'apperçu sur le bord droit pour la laisser ouverte en parallèle du code
+++
---

### 1 - PlatformIO & la librairie Scheduler

Voici les pages web à consulter pour une aide sur les sujets suivants :
- [L'installation et l'utilisation de l'extension **PlatformIO**](https://doc.payet.top/scheduler/platformio.md).

- [L'organisation générale du dossier de ce projet](https://doc.payet.top/scheduler/projet.md).

- [Sur l'utilisation de la librairie Scheduler](https://doc.payet.top/scheduler/chapitre%201%20-%20structure%20du%20code.md).

### 2 - Procédure de connexion USB du robot

#### Pour brancher le robot :
1. Allumer le robot avec son bouton on/off (on doit toujours commencer par l'alimenter sur sa batterie).
2. Attendre 1 ou 2 secondes le temps que l'Arduino stabilise ses broches.
3. Brancher le cordon USB.

#### Pour débrancher le robot :
1. Débrancher le câble USB.
2. Mettre le robot sur OFF.


### 3 - NB :

- pour consulter la charge de la batterie il suffit de créer un objet `SensorINA219` :
```cpp
#include "sensor/Ina219.h"

//...

SensorINA219 capteurINA219;   //pour l'affichage des données du capteur de tension/courant
```
Cet objet est une tâche qui consulte le capteur de tension **Ina219** toutes les 2 secondes, pour afficher un bilan énergétique dans la console. La dernière ligne de ce bilan donne le pourcentage de batterie restant.

Notez que si ce bilan montre une tension inférieure à 6v c'est que le robot n'est pas sur ON (il est alimenté uniquement par le port USB). Vous n'avez donc pas suivit la procédure décrite dans la section 2 ci-dessus.
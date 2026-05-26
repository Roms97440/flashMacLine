---
title: (FMcL) Aide générale
---
## Aide générale (important) 

<!-- #region (à replier) -->
<details open><summary>(si vous consultez ce fichier dans **VSCode**)</summary>

Pour améliorer le confort de lecture :
- clic-droit sur le fichier dans le volet d'exploration (l'explorateur à gauche)
- choisir "Ouvrir l'aperçu" (*Open preview*)
- glisser ensuite la fenêtre d'aperçu sur le bord droit pour la laisser ouverte en parallèle du code

Notez que cette [documentation est consultable en ligne sur cette page](https://doc.payet.top/flashmcline/_plan_.md)
</details>
<!-- #endregion -->

---

### 1 - Recommandantions :
- Ce projet doit être ouvert avec **VSCode** en utilisant le fichier workspace : `flashMacLine.code-workspace`. Donc, dans **VSCode** :
    - menu Fichier->"Ouvrir l'espace de travail à partir du fichier..."
    - puis, sécelectionnez le fichier `flashMacLine.code-workspace` (en racine du dossier du projet)

- Toujours faire un `pull` à l'ouverture de ce projet. Et ne pas oublier de faire un `push`quand vous avez terminé vos modifications. 

- (facultatif) il est conseillé d'installer l'extension **VSCode** : `#region folding`. Il permet de replier les zones de code délimitées par les marqueurs en commentaire : `/* #region ... */` et `/* #endregion */`. C'est très pratique pour obtenir une vue d'ensemble du code.

### 2 - PlatformIO & la librairie Scheduler

Voici les pages web à consulter pour une aide sur les sujets suivants :
- [L'installation et l'utilisation de l'extension **PlatformIO**](https://doc.payet.top/scheduler/platformio.md).

- [L'organisation générale du dossier de ce projet](https://doc.payet.top/scheduler/projet.md).

- [Sur l'utilisation de la librairie Scheduler](https://doc.payet.top/scheduler/chapitre%201%20-%20structure%20du%20code.md).

### 3 - Procédure de connexion USB du robot

#### Pour brancher le robot :
1. Allumer le robot avec son bouton on/off (on doit toujours commencer par l'alimenter sur sa batterie).
2. Attendre 1 ou 2 secondes le temps que l'Arduino stabilise ses broches.
3. Brancher le cordon USB.

#### Pour débrancher le robot :
1. Débrancher le câble USB.
2. Mettre le robot sur OFF.


### 4 - Gestion de la batterie

Pour consulter la charge de la batterie il suffit de créer un objet `SensorINA219` :
``` cpp
#include "sensor/Ina219.h"

//...

SensorINA219 capteurINA219; //affichage des données du capteur de tension/courant
```
Cet objet est une tâche qui consulte le capteur de tension **Ina219** toutes les 2 secondes, pour afficher un bilan énergétique dans la console. La dernière ligne de ce bilan donne le pourcentage de batterie restant.

Notez que si ce bilan montre une tension inférieure à 6v c'est que le robot n'est pas sur ON (il est alimenté uniquement par le port USB). Vous n'avez donc pas suivit la procédure décrite dans la section 3 ci-dessus.

A titre indicatif : 
 - une charge complête permet au robot de fonctionner pendant 5 à 6h
 - quand la batterie est à 10%, le robot peut encore fonctionner 10 à 15 minutes. Mais il est préférable de recharger les batteries quand on passe en dessous des 20%.

Afin d'aider à cette surveillance, vous pouvez inclure dans vos programmes la tâche `LedBat` (définie dans le fichier `sensor/Ina219.h`) :
``` cpp
#include "Pindef.h" //definitions des pins du robot
#include "sensor/Ina219.h"

//...

SensorINA219 capteurINA219; //affichage des données du capteur de tension/courant
LedBat ledBat(PIN_LED_ROUGE, capteurINA219); //-> led rouge == alerte batterie

//...
```

Cette tâche pilote la led qui lui est attribué (ici la led rouge) pour signaler le niveau de charge de la batterie :
- led éteinte : **niveau** >= 40%
- led glinotante : 40% > **niveau** >= 20%
- led allumée fixe : **niveau** < 20%  (il faut charger la batterie dans les 10 prochaines minutes !)

Mais notez que l'usage de cette tâche monopolise une led. Comme la verte est dédiée à l'état ON/OFF, cela veut dire qu'il ne restera plus qu'une seule led disponible pour votre programme (la jaune dans le code ci-dessus).

<br/>

---
(retour à [la page d'accueil](./_plan_.md))
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

### 3 - Mises à jour, et vérification des dépendances 

Comme vous l'aurez compris, le proget global comporte plusieurs sous-projet qui correspondent chacun à un projet PlatformIO (la librairie commune `hardwareLib`, et les différents programmes à uploader vers le robot). Aussi, il est important de vérifier que ces sous-projet soient tous bien ajustés en ce qui concerne leurs dépendances (surtout si vous rencontrez des erreurs de compilation qui découlent possiblement d'un souci à ce niveau).

Ce qu'il faut vérifier en particulier c'est que :
 - le projet de la lirbairie `hardwareLib` est bien calé sur la dernière version de la lirbairie `Scheduler`.
 - que les projets des programmes à uploader sont bien calés sur les bonnes versions des 2 librairies : `hardwareLib` et `Scheduler`.

Voici comment vérifier tout ça dans **VSCode**, et comment corriger en cas de souci :

#### - Pour la librairie `hardwareLib` :
- ouvrez l'un des fichier de cette librairie (par exemple `src/Pindef.h`)
- cliquez ensuite sur le bouton `New Terminal` dans la barre d'action de **PlatformIO** (tout en bas)
- cela va ouvrir une console PIO pour ce projet (le prompt doit commencer par `hardwareLib %`)
- dans cette console tapez la commande : 
```
pio pkg list
```
- sur la dernière ligne affiché, vous devriez voir quelque chose qui commence par : `Scheduler @ 0.20.5 ...`
    - dans cet exemple, les chiffre `0.20.5` indique que ce projet intègre la version 0.20.5 de la librairie `Scheduler`.
    
- allez maintenant sur le dépôt Git de la librairie `Scheduler` pour vérifier sa version actuelle : [https://gitlab.com/c-arduino/scheduler](https://gitlab.com/c-arduino/scheduler).
- si vous n'avez pas la dernière version:
    - tapez dans la console : `pio pkg update` puis `pio pkg list`. 
    - Tapez ensuite une nouvelle fois ces 2 commandes : `pio pkg update` puis `pio pkg list` (il faut le faire deux fois pour que ce soit pris en compte).
    - vous devriez maintenant voir le bon numéro de version s'afficher pour la librairie `Scheduler`.

#### - Pour les autres sous-projets (chacun des programmes à uploader) :
Cela fonctionne de la même façon pour vérifier si le numéro de version est bon. Mais la procédure de correction est différente ici.
- ouvrez l'un des fichier du sous-projet (par exemple `plateformio.ini`)
- cliquez ensuite sur le bouton `New Terminal` dans la barre d'action de **PlatformIO** (tout en bas)
- cela va ouvrir une console PIO pour ce projet là (le prompt doit commencer par `NOM_DU_SOUS-PROJET %`)
- dans cette console tapez la commande : 
```
pio pkg list
```
- comme ci-dessus, la dernière ligne affiche la version de la librairie `Scheduler` pris en compte pour ce sous-projet.
- ici, si vous n'avez pas la dernière version de la librairie `Scheduler` vous devez taper ceci dans la console :
```
pio pkg update --library "https://gitlab.com/c-arduino/scheduler.git"
```
 - tapez une nouvelle fois cette même commande (la aussi, il faut le faire deux fois pour que ce soit pris en compte).
Ensuite, si vous tapez à nouveau `pio pkg list`, vous devriez voir le bon numéro de version s'afficher.

### 4 - Procédure de connexion USB du robot

#### Pour brancher le robot :
1. Allumer le robot avec son bouton on/off (on doit toujours commencer par l'alimenter sur sa batterie).
2. Attendre 1 ou 2 secondes le temps que l'Arduino stabilise ses broches.
3. Brancher le cordon USB.

#### Pour débrancher le robot :
1. Débrancher le câble USB.
2. Mettre le robot sur OFF.


### 5 - Gestion de la batterie

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
LED_BAT(ledBat, PIN_LED_ROUGE, capteurINA219, "Led Rouge -> batterie");

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
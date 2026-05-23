## Listing des programmes

+++ (pour consulter ce fichier)
- clic-droit sur le fichier dans le volet d'exploration (l'explorateur à gauche)
- choisir "Ouvrir l'aperçu"
+++
---

<span style='color: red'>**Attention :** </span> ne pas oublier de choisir le projet cible (bouton Switch dans la barre d'action de **PlatformIO** tout en bas), avant de compiler/uploader.


---
Voici la liste des différents programmes (donc des sous projets **PlatformIO**) :

- **Draft** : zone de bac à sable (pour expérimenter du codage divers).
    - hello : programme minimaliste pour vérifier le bon fonctionnement de la compilation et de l'upload.
    - testSchedule : exemple de code simple pour illustrer l'usage du scheduleur. Notamment avec un code répartie dans 2 fichiers sources.

<br/>

- **Lib** : les librairies locales communes à tous les programmes.
    - **hardwareLib** : contient les classes modilisant chacun des composants du robot, et le fichier de définition du câble : `Pindef.h`.

<br/>

- **Test** : les programmes permettants de faire les vérifications du bon fonctionnement des éléments du robot.
    - TestAccessoire : permet de tester les leds, les boutons, le buzzer, et le capteur de courant Ina219.
    - TestRoue : permet de tester le bon fonctionnement des roues (donc des 2 moteurs).
    - TestCapteur : permet de tester le bon fonctionnement du capteur de ligne (Qtr-3RC).

<br/>

- **Progs** : les programmes généraux (dont le programme final...).
    - Algo1 : test d'un premier algo de pilotage basique.

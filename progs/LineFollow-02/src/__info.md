## LineFollow-02 : v2.00

<center style='color: orange;'>

### /!\ vous devez mettre à jour la librairie Scheduler vers v0.24 ou + /!\

</center>

---

### 1 - Changements par rapport à LineFollow (v1)

Ce deuxième essais reprend les mêmes bases que le programme initial `LineFollow` mais avec les ajustements suivants :

#### 1.1 - Ajustements déjà en place :

- La tâche `taskForward`, déclenchée sur l'action 3 (via l'appui simultané des 2 boutons), est remplacée par la tâche `testCalibrage` (voir la **section 2.3** ci-dessous).
- Le lancement d'action (via l'appui sur les boutons) est maintenant confirmé par un nombre de bip correspondant au numéro de l'action : 
  - 1 bip => action 1 (Bouton gris) => lance la calibration
  - 2 bip => action 2 (Bouton bleu) => lance le suivi de ligne
  - 3 bip => action 3 (les 2 boutons) => lance le test du calibrage

- La fin d'une action (calibration, suivi de ligne, test du calibrage) est signalée par un son buzz. Cette annulation peut se faire :
  - pour la calibration : à la fin du processus, ou avant en appuyant sur n'importe quel bouton.
  - pour le suivi de ligne : à tout moment en appuyant sur n'importe quel bouton.
  - pour le test du calibrage : en appuyant sur les 2 boutons en même temps.

- Optimisation du code interne du capteur de ligne :
  - la charge CPU durant la calibration passe de 90% à 36%.
  - la charge CPU pour la lecture du capteur (après calibration) passe de 16% à 8%.

- L'exécution de la tâche de suivie de ligne (TaskFollow) est maintenant liée à celle de la tâche de relevé de mesure du capteur de ligne (SensorQTR_3RC) : l'alogo de suivi de ligne sera donc toujours exécutée après la capture de la dernière mesure.

- Suppresion de la configuration LIGH_PERIOD (plus nécessaire après les 2 optimisations ci-dessus).

- Réorganisation des classes pour les moteurs (`Motor.h`) : le gestionnaire **BiMotor** peut maitenant gérer des moteurs de n'importe quel type (basic, smoth, ect...), et dispose désormais de toutes les possibilités d'action sur ces moteurs évitant ainsi de devoir les manipuler en plus de l'objet **BiMotor** dans le code.

- Optimisation et lissage du code de la tâche de suivi de ligne (sans changer l'algo).

- Ajout de la tâche Gardian (fichier `06_gardian.cpp`) : cette tâche essais de détecter les situations critiques dans lesquelles peut se trouver le robot,
  afin de donner l'alerte pour que l'algo de pilotage puisse réagir de façon appropriée. Pour le moment, quand l'alerte est détectée : elle est signalée par un son et la led jaune, et la tâchde de suivi de ligne est simplement arrêtée. Si lors des essais au sol la détection est un succés, le code sera complété pour déclencher des vrais réaction sur le pilotage.

#### 1.2 - Prochains ajustement (en cours de codage) :

- Ajout d'un moteur à impulsion : similaire aux moteurs smoth, mais avec un système d'impulsion pour les vitesses lentes afin de rouler très lentement sans être bloqué (pour les vitesses en dessous du seuil d'arrachement).

### 2 - Modalité de lancement des 3 actions :
 
 - Bouton gris (BT1) => lance la calibration (voir protocole ci-dessous).
 - Bouton bleu (BT2) => lance le suivi de ligne.
 - Les 2 boutons en même temps => test du calibrage (*nouveau*).

#### 2.1 -  Protocole de calibration  (le bouton GRIS -> 1 bip) :
- placez le robot au sol, perpendiculairement à la ligne, le capteur juste avant la ligne (sur du blanc).
- appuyez sur le bouton GRIS. Un bip retentit, et la led jaune va clignoter pendant 5 secondes.
- la calibaration commence. La led jaune reste fixe durant toute la calibration.
  (le robot fait des mouvements avant/arrière pour passer plusieurs fois sur le capteur)
- au bout des 10 secondes : la led jaune s'etteind, le robot émet un buzz, et la calibration est terminée.
- pour annuler cette tâche avant la fin : appuyez sur n'importe quel bouton  (un buzz retentit).

#### 2.2 -  Protocole de suivi de ligne  (le bouton BLEU -> 2 bip) :
- ne jamais lancer cette tâche sans avoir fait une calibration (voir section 2.1).
- placez le robot au sol, au début du parcours.
- appuyez sur le bouton BLEU (un double bip retentit), et le robot avance en suivant la ligne.
- pour mettre fin à cette tâche : appuyez sur n'importe quel bouton (un buzz retentit).

#### 2.3 - Protocole du test de calibrage (les 2 boutons en même temps -> 3 bip) :
- appuyez sur les 2 boutons en même temps pour activer cette tâche (un tripple bip retentit)
- notez quand dans cette tâche les roues ne tournent jamais.
- placez le robot au sol, proche de la ligne noire, en le tenant face à vous (vous regardez les 2 triangles en bas du capot avant).
- positionnez la ligne noire entre les triangles, ou volontairement désaxé par rapport à eux.
- appuyez sur le bouton BLEU pour demander une mesure, un son de notification long retentit et les led jaune et/ou rouge s'allument pour indiquer la valeur de déviation (voir tableau ci-dessous).
- faites des nouvelles mesures en décalant la ligne noire.
- pour effacer la mesure (les led vont s'etteindre toutes les deux) : appuyez sur le bouton GRIS (deux son de notification retentissent)
- pour quitter cette tâche : appuyez en même temps sur les 2 boutons (un buzz retentit).

Vue face à la voiture (l'avant du capot nous fait face), suite à la demande d'une capture (bouton BLEU) :
 -> la valeur du capteur va être lue (1ère colonne).
 -> celle-ci va être traduite en allumage/clignotement des leds jaune/rouge.
 -> consultez la signification (la ligne noire est *XXX*) dans la dernière colonne.
| Valeur lue | Etat des leds | Signification |
| --- | --- | --- |
| [-1000; -700[ | jaune blink | trop à gauche |
| [-700; -400[ | jaune fixe | à gauche |
| [-400; -100[ | jaune fixe + rouge blink | légèrement à gauche |
| [-100; 100] | jaune fixe + rouge fixe | au centre |
| ]100; 400] | jaune blink + rouge fixe | légèrement à droite |
| ]400; 700] | rouge fixe | à droite |
| ]700; 1000] | rouge blink | trop à droite |

**NB:** pendant l'exécution de cette tâche, la tâche **LedBat** (avertisseur de batterie) est désactivée pour libérer l'usage de la led rouge.


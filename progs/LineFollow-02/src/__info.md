## LineFollow-02 : v2.00

<center style='color: orange;'>

### /!\ vous devez mettre à jour la librairie Scheduler vers v0.24 ou + /!\

voir la **section 3** sur cette page : [https://doc.payet.top/flashmcline/00_aide.md](https://doc.payet.top/flashmcline/00_aide.md)
</center>

---

### 1 - Changements par rapport à LineFollow (v1)

Ce deuxième essais reprend les mêmes bases que le programme initial `LineFollow` mais avec les ajustements suivants :

- (A) La tâche `taskForward`, déclenchée sur l'action 3 (via l'appui simultané des 2 boutons), est remplacée par la tâche `testCalibrage` (voir la **section 2.3** ci-dessous).
- (B) Le lancement d'action (via l'appui sur les boutons) est maintenant confirmé par un nombre de bip correspondant au numéro de l'action : 
  - 1 bip => action 1 (Bouton gris) => lance la calibration
  - 2 bip => action 2 (Bouton bleu) => lance le suivi de ligne
  - 3 bip => action 3 (les 2 boutons, relachement du bleu en 1er) => lance le test du calibrage
  - 4 bip => action 4 (les 2 boutons, relachement du gris en 1er) => lance la tache sniffer (pour tester)

- (C) La fin d'une action (calibration, suivi de ligne, test du calibrage) est signalée par un son buzz. Cette annulation peut se faire :
  - pour la calibration : à la fin du processus, ou avant en appuyant sur n'importe quel bouton.
  - pour le suivi de ligne : à tout moment en appuyant sur n'importe quel bouton.
  - pour le test du calibrage : en appuyant sur les 2 boutons en même temps.
  - pour un essaus de la tâche sniffer : à tout moment en appuyant sur n'importe quel bouton.

- (D) Optimisation du code interne du capteur de ligne :
  - la charge CPU durant la calibration passe de 90% à 36% (voir possiblement à 20%).
  - la charge CPU pour la lecture du capteur (après calibration) passe de 16% à 8%.

- (E) L'exécution de la tâche de suivie de ligne (TaskFollow) est maintenant liée à celle de la tâche de relevé de mesure du capteur de ligne (SensorQTR_3RC) : l'algo de suivi de ligne sera donc toujours exécuté après la capture de la dernière mesure.

- (F) Suppresion de la configuration LIGH_PERIOD (plus nécessaire après les 2 optimisations ci-dessus).

- (G) Réorganisation des classes pour les moteurs (`Motor.h`) : le gestionnaire **BiMotor** peut maitenant gérer des moteurs de n'importe quel type (basic, smoth, ect...), et dispose désormais de toutes les possibilités d'action sur ces moteurs évitant ainsi de devoir les manipuler séparément en plus de l'objet **BiMotor** dans le code.

- (H) Augmentation de la réactivité de base des moteurs Smoth. De plus, la valeur du **step** (le pas de convergence de la vitesse vers celle demandée) peut maintenant être fixée à la création de l'objet moteur. Cette possibilité est utilisée dans le code pour définir le type de moteur **Réactif** dont le step est fixé à son maximum 30. Pour choisir ce type de moteur il vous suffit de sélectionner `#define USE_MOTOR _REACTIF` dans le fichier `00_config.h`.

- (I) Optimisation et lissage du code de la tâche de suivi de ligne (sans changer l'algo). Des blocs `region folding` auto-fermés ont été ajoutés pour permettre une vue directe sur le code de l'algo et ses constantes de réglage.

- (J) Ajout de la tâche **Gardian** (fichier `06_gardian.cpp`) : cette tâche essais de détecter les situations critiques dans lesquelles peut se trouver le robot, afin de donner l'alerte pour lancer une réaction appropriée. Quand une alerte est détectée elle est signalée par un son et la led jaune, et la tâche de suivi de ligne est mise en pause. La réaction à l'alerte est la suivante :
  - s'il s'agit d'une alerte "perte de la ligne noire" : un son buzz long se produit, et la led jaune clignote. Si la tâche Sniffer (voir (K) ci-dessous) est activée dans la configuration, elle est lancée. Sinon, le robot restera comme ça avec la led jaune clinotante (relancez la tâche de suivi de ligne pour revenir à la normale).
  - s'il 'agit d'une alerte "robot immobilisé" : 2 son buzz moyen se produisent, et la led jaune reste fixe. Un boost direct (il ne s'agit pas la du booster `05_booster.cpp`, donc le mode boost peut être laissé désactivé dans `00_config.h`) est alors appliqué, et devient de plus en plus instense si le blocage persiste. Au bout d'un certains seuil d'intensité (réglable dans le code juste avant la classe Gardian), le robot coupera les moteurs et restera comme ça avec la led jaune fixe (relancez la tâche de suivi de ligne pour revenir à la normale).

- (K) Ajout de la tâche Sniffer (fichier `07_sniffer.cpp`) : cette tâche essais de retrouver la ligne noire en réalisant des mouvements de rotation droite-gauche de plus en plus large. Le calibrage de cette tâche peut être réalisé en ajustant les constantes de réglages dans le code juste avant la classe Sniffer. Cette tâche peut aussi être testée de façon isolée via le déclenchement de l'action 4 (voir section 2.4).

- (L) Les réglages des paramètres et des constantes se font à 2 niveaux :
  - réglages généraux : dans le fichier `00_config.h` (il n'est plus nécessaire de modifier le fichier `01_main.cpp`). En général c'est des `#define` à commenter ou à décommenter.
  - réglages spécifiques à une tâche : dans le fichier d'implémentation de la tâche, juste avant le code de sa classe (zone marquée par `// == constantes de réglage ==` et ). En général c'est des valeurs à changer pour des constantes `constexpr`.

A noter que les nouvelles tâches **Gardian** et **Sniffer** peuvent être activées/désactivées dans `config.h`. Si elles sont toutes les 2 désactivées, ce programme **LineFollow-02** se comportera alors de façon identique avec la version 1, avec juste les optimisations (A) à (I).

### 2 - Modalité de lancement des 4 actions :
 
 - Bouton gris (BT1) => lance la calibration (voir protocole ci-dessous).
 - Bouton bleu (BT2) => lance le suivi de ligne.
 - Les 2 boutons en même temps et relachement du bleu en 1er => test du calibrage (*nouveau*).
 - Les 2 boutons en même temps et relachement du gris en 1er =>  lance la tache sniffer pour tester la recherche de ligne (placez le robot à droite ou à gauche de la ligne).

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

#### 2.3 - Protocole du test de calibrage (les 2 boutons en même temps + bouton bleu relaché en 1er -> 3 bip) :
- appuyez sur les 2 boutons en même temps pour activer cette tâche, et relacher le bouton bleu en 1er (un tripple bip retentit)
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

#### 2.4 - Protocole du test Sniffer (les 2 boutons en même temps + bouton gris relaché en 1er -> 4 bip) :

- assurez-vous que la tâche **Sniffer** est bien acvtivée dans  `00_config.h`.
- réalisez une calibration.
- placez le robot au sol, prôche de la ligne ou dans une position ou vous souhaitez expérimenter une recherche de ligne.
- activez cette tâche : appuyez sur les 2 boutons en même temps et relachez le bouton gris en 1er (4 bip retentissent).
- durant cette tâche la led jaune clignote, et le robot va faire des rotations successive à droite et à gauche de plus en plus large.
- Fin du test :
  - si le robot trouve la ligne : un son bip long se produit, la led jaune s'éteind, et la tâche de suivi de ligne est activée (vous pouvez l'annuler en appuyant sur n'importe quel bouton).
  - si le robot termine sa recherche sans avoir trouvé la ligne :  le robot d'arrête, un son buzz long produit, et la led jaune reste fixe.

NB: pour annuler cette tâche il suffit de lancer la tâche de suivi de ligne (Bouton bleu). Et éventuellement annuler ensuite ce suivi en appuyant sur n'importe quel bouton.
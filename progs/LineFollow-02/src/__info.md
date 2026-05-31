## LineFollow-02 : v2.00

<center style='color: orange;'>

### /!\ vous devez mettre à jour la librairie Scheduler vers v0.23.2 ou + /!\

</center>

---

### 1 - Changements par rapport à LineFollow (v1)

Ce deuxième essais reprend les mêmes bases que le programme initial `LineFollow` mais avec les ajustements suivants :

- La tâche `taskForward`, déclenchée sur l'action 3 (via l'appui simultané des 2 boutons), est remplacée par la tâche `testCalibrage` (voir la **section 2.3** ci-dessous).


### 2 - Modalité d'usage avec le robot :
 
 - Bouton gris (BT1) => lance le calibrage (voir protocole ci-dessous)
 - Bouton bleu (BT2) => lance le suivi de ligne
 - Les 2 boutons en même temps => test du calibrage (*nouveau*).

#### 2.1 -  Protocole de calibration  (le bouton GRIS -> 1 bip) :
- placez le robot au sol, perpendiculairement à la ligne, le capteur juste avant la ligne (sur du blanc)
- appuyez sur le bouton GRIS. Un bip retentit, et la led jaune va clignoter pendant 5 secondes.
- la calibaration commence. La led jaune reste fixe durant toute la calibration.
  (le robot fait de mouvement avant/arrière pour passer plusieurs fois sur le capteur)
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
- positionnez la ligne noire entre les traingles, ou volontairement désaxé par rapport à eux.
- appuyez sur le bouton BLEU pour demander une mesure, un son de notification long retentit et les led jaune et/ou rouge s'allument pour indiquer la valeur de déviation (voir tableau ci-dessous).
- faites des nouvelles mesures en décalant la ligne noire
- pour effacer la mesure (les led vont s'etteindre toutes les deux) : appuyez sur le bouton GRIS (deux son de notification retentissent)
- pour quitter cette tâche : appuyez en même temps sur les 2 boutons (un buzz retentit).

Vue face à la voiture (l'avant du capot nous fait face), suite à la demande d'une capture (bouton BLEU) :
 -> la valeur du capteur va être lue (1ère colonne)
 -> celle-ci va être traduite en allumage/clignotement des leds jaune/rouge
 -> consultez la signification (la ligne noire est *XXX*) dans la la dernière colonne.
| Valeur lue | Etat des leds | Signification |
| --- | --- | --- |
| [-1000; -700[ | jaune blink | trop à gauche |
| [-700; -400[ | jaune fixe | à gauche |
| [-400; -100[ | jaune fixe + rouge blink | légèrement à gauche |
| [-100; 100] | jaune fixe + rouge fixe | au centre |
| ]100; 400] | jaune blink + rouge fixe | légèrement à droite |
| ]400; 700] | rouge fixe | à droite |
| ]700; 1000] | rouge blink | trop à droite |

NB: pendant l'exécution de cette tâche, la tâche LedBat (avertisseur de batterie) est désactivée pour libérer l'usage de la led rouge.


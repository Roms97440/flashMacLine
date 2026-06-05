## LineFollow : v0.1.00

<center style='color: orange;'>

### /!\ vous devez mettre à jour la librairie Scheduler vers v0.23.2 ou + /!\

voir la **section 3** sur cette page : [https://doc.payet.top/flashmcline/00_aide.md](https://doc.payet.top/flashmcline/00_aide.md)

</center>

---

Premier essais d'un programme de suivi de ligne avec la nouvelle architecture du code de la librairie `hardwareLib` (et la dernière version de la librairie `Scheduler`).

### Modalité d'usage avec le robot :
 
 - Bouton gris (BT1) => lance le calibrage (voir protocole ci-dessous)
 - Bouton bleu (BT2) => lance le suivi de ligne
 - Les 2 boutons en même temps => lance testForward (avance en ligne droite).


#### -  Protocole de calibration :
- placer le robot au sol, perpendiculairement à la ligne, le capteur juste avant la ligne (sur du blanc)
- appuyer sur le bouton GRIS. La led jaune va clignoter pendant 5 secondes.
- la calibaration commence. La led jaune reste fixe durant toute la calibration.
  (le robot fait de mouvement avant/arrière pour passer plusieurs fois sur le capteur)
- au bout des 10 secondes : la led jaune s'etteind, le robot émet 3 bip, et la calibration est terminée.


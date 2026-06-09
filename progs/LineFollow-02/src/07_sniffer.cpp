//Fichier source secondaire: 
#include "00_config.h"

//NB : classe en cours de codage : NON FINALISé !

/*  ===> Sniffer est une tâche qui essais essais de retrouver la ligne via de mouvement circulaire droite-gauche de plus en plus grand.
    -> Chaque rotation dans un sens est appelé "une phase".

  Idéalement cette tâche doit être activé quand le Gardian détecte que le robot a perdue la ligne depuis plus d'une seconde.
  -> elle se désactive automatiquement quand la tâche de suivi de ligne est activé

  - quand cette tâche est active la led jaune clignote.

  Cette tâche se termine dans les 2 cas possibles suivants :
  - la ligne noire a été retrouvé :  1 son bip long se produit, et la led s'éteind 
      => sera complété plus tard par une reprise de la tâche TaskFollow
  - la ligne noire n'a pas été trouvé : 1 son buzz long produit, et la led jaune reste fixe.

==> pour annuler cette tâche il suffit de relancer la tâche de suivi de ligne.

*/
//=====================
#ifdef ENABLE_SNIFFER

/* #region(close) les #include */
#include "sensor/Button.h"
#include "actuator/Led.h"
#include "actuator/Motor.h"
#include "actuator/Buzzer.h"
#include "sensor/Qtr-3RC.h"
/* #endregion */

/* #region(close) les objets définis dans les autres fichiers */
class TaskFollow : public Task {};
extern BiButton biButton;
extern Buzzer buzzer;
extern SensorQTR_3RC capteur;
extern BiMotor motors;
extern Task* ptrTaskFollow;
extern Led ledJaune;  //pour la led jaune (définit dans 02_calibragtion.cpp)
/* #endregion */

// == constantes de réglage =============
constexpr uint8_t speedTS = 80; //vitesse des rotations 
constexpr uint8_t catchWin = 4; //nombre de capture positive de la ligne concécutive avant de conclure qu'on l'a retrouvé
constexpr uint8_t maxPhase = 8; //nbr de phase maximum (1 phase = rotation dans un sens)
constexpr int16_t stepPhase = 100; //durée du pas des phases de rotation en nombre de cycle de 20 ms.  100 => 2s
constexpr bool followIfFound = true; //mettre à false si le robot doit simplement tout arrêter quand il a retrouvé la ligne
// ====  ====  ====  ====  ====  ====  ==== 

class TaskSniffer : public Task {
 public :
   enum Movement { NONE, LEFT, RIGHT };  
 protected :
    Movement _move;     //mouvement en cours
    int16_t _count;     //compteur de cycle pour chronométrer les rotations
    int16_t _endCount;  //valeur du compteur marquant la fin de la phase courant
    uint8_t _phase;     //numéro de la phase de rotation actuelle
    uint8_t _catch;     //nombre de capture concécutive de la ligne
    uint8_t _swap;      //pour le clignotement de la led jaune

 public :
   SETNAME("TaskSniffer") //nom affiché dans le bilan de lancement de RMonitor
   TaskSniffer() : Task(capteur, false), _move(NONE), _count(0), _endCount(0), _phase(0), _catch(0), _swap(0) {} //cette tâche va suivre la rythmique du capteur de ligne 
      //---> elle sera tjs exécutée après la tâche de relevé de mesure, soit toutes les 20ms
   void run() override {  
      if(ptrTaskFollow->isEnabled()){ //annulation de la tâche
        ledJaune.setOn(false);
        _move=NONE;
        setEnabled(false);
        return;
      }

      if(_move==NONE){ //on commence la recherche
        _count=0;
        _phase=0;
        _endCount=stepPhase; //sur le 1er mouvement 1 seul stepPhase à atteindre
        _catch=0;
        //on commence la recherche dans le sens ou la ligne à était vue la dernière fois
        _move = capteur.deviation()>0 ? LEFT : RIGHT; //TODO à tester pour voir s'il faut inverser ! 
        _swap=0;
      } else {
        if(capteur.lostLine()){ //On a pas retrouvé la ligne ? 
          _catch=0;
          _count++;
          if(_count>=_endCount) _nextPhase(); //fin de cette phase
          else motors.rotate(_move==RIGHT ? MV_RIGHT : MV_LEFT, speedTS);
        } else { //on a capturé la ligne
          _catch++;
          if(_catch>=catchWin){ //victoire !
            motors.stop();
            buzzer.bip(1500);
            ledJaune.setOn(false);
            _move=NONE;
            setEnabled(false);
            if(followIfFound) ptrTaskFollow->setEnabled(true);
          }
        }

        //clignotement de la led jaune
        _swap++;
        if(_swap>=25){
          ledJaune.swap();
          _swap=0;
        }
      }
   }

   void _nextPhase(){ //passage à la phase suivante
      motors.stop();
      _phase++;
      if(_phase>=maxPhase){ //on a terminé la dernière phase sans trouver :(
        buzzer.buzz(1500);
        ledJaune.setOn(true);
        _move=NONE;
        setEnabled(false);
      } else {
        _move = _move==LEFT ? RIGHT : LEFT; //on change de sens
        _count=0;
        _endCount=stepPhase*2; //sur les autres mouvement que le 1er on doit décompter 2 stepPhase pour une rotation complête dans l'autre sens
      }
   }

   void start(){
    if(!isEnabled()){
      buzzer.bip(400, 4, 200);
      motors.stop();
      _move=NONE;
      setEnabled(true);
    }
   }

   void stop(){
    ledJaune.setOn(false);
    if(isEnabled()){
      buzzer.buzz(1500);
      motors.stop();
      _move=NONE;
      setEnabled(false);   
    }
   }
};
TaskSniffer taskSniffer;
Task* ptrTaskSniffer = &taskSniffer; //pour la tâche Gardian

SET_ACTION(setActionTaskSniffer, biButton, ALL_BT1R, [](bool launch, uint8_t bt){ //action : 2 boutons, puis relâche du gris
     if(launch) {
      taskSniffer.start();
      return true;
     } else {
      taskSniffer.stop();
      return false;
     }
});

#endif
//=====================
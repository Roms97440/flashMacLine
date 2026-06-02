//Fichier source secondaire: 
#include "00_config.h"

//NB : classe en cours de codage : NON FINALISé !

/*  ===> Sniffer est une tâche qui essais essais de retrouver la ligne via de mouvement circulaire droite-gauche de plus en plus grand.

  Idéalement cette tâche doit être activé quand le Gardian détecte que le robot a perdue la ligne depuis plus d'une seconde.
  
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

class TaskSniffer : public Task {
 public :
   enum Movement { NONE, LEFT, RIGHT };  
 protected :
    Movement _move;     //mouvement en cours
 public :
   SETNAME("TaskSniffer") //nom affiché dans le bilan de lancement de RMonitor
   TaskSniffer() : Task(capteur, true, true), _move(NONE) {} //cette tâche va suivre la rythmique du capteur de ligne 
      //---> elle sera tjs exécutée après la tâche de relevé de mesure, soit toutes les 20ms
   void run() override {  
      if(ptrTaskFollow->isEnabled()){ //annulation de la tâche
        ledJaune.setOn(false);
        _move=NONE;
        setEnabled(false);
        return;
      }

      //TODO...
   }
};
TaskSniffer taskSniffer;

#endif
//=====================
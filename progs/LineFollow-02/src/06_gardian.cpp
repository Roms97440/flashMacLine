//Fichier source secondaire: 
#include "00_config.h"

/*  ===> Gardian est une tâche qui essais de détecter les situations critiques dans lesquelles peut se trouver le robot,
      afin de donner l'alerte pour que l'algo de pilotage puisse réagir de façon appropriée.

    Les situations critique sont les suivantes (par ordre de priorité fort->faible, seule la situation la plus prioritaire est signalée):
    - le robot a perdue la ligne depuis plus d'une seconde. 
    - le robot est immobile depuis plus de 2 secondes. 

    Pour activer cette tâche la ligne suivante doit être décommenté dans `00_config.h`

NB: pour l'instant le comportement du Gardian en cas d'alerte est le suivant :
- alerte perte de ligne : si la tâche de suivi de ligne est active, elle est désactivér et les moteurs sont coupés. 
---> Puis : 1 son buzz long se produit, et la led jaune clignote.

- alerte robot immobile : si la tâche de suivi de ligne est active, elle est désactivér et les moteurs sont coupés. 
---> Puis : 2 son buzz moyen se produisent, et la led jaune reste fixe.

==> pour annuler l'alerte il suffit de relancer la tâche de suivi de ligne

*/
//=====================
#ifdef ENABLE_GARDIAN

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

class TaskGardian : public Task {
 public :
   enum Alerte { NONE, LOST, BLOCKED };  
 protected :
    uint8_t _countLost;    //compteur de perte de ligne -> alerte s'il arrive à 50 (=> 1 secondes)
    uint8_t _countBlocked; //compteur robot immobile -> alerte s'il arrive à 100 (=> 2 secondes)
    int16_t _lastDeviation;
    Alerte _alerte;     //alerte actuelle
 public :
   SETNAME("TaskGardian") //nom affiché dans le bilan de lancement de RMonitor
   TaskGardian() : Task(capteur, true, true),  _countLost(0), _countBlocked(0), _lastDeviation(2000), _alerte(NONE) {} //cette tâche va suivre la rythmique du capteur de ligne 
      //---> elle sera tjs exécutée après la tâche de relevé de mesure, soit toutes les 20ms
   void run() override {  
      switch(_alerte){
        case NONE:
          //analyse de l'alerte LOST
          if(ptrTaskFollow->isEnabled() && capteur.lostLine()) {
            _countLost++;
            if(_countLost>=50){ //alerte de perte de ligne
              _alerte=LOST;
              ptrTaskFollow->setEnabled(false); //on arrête la tâche de suivi de ligne
              motors.stop(); //on coupe les moteurs
              buzzer.buzz(1500); //buzz long
              _countLost=0;
              break; //on sort du switch
            }
          } else _countLost=0;
          
          { //analyse de l'alerte BLOCKED
            int16_t newDeviation=capteur.deviation();
            if(ptrTaskFollow->isEnabled() && _lastDeviation==newDeviation){
              _countBlocked++;
              if(_countBlocked>=100){ //alerte robot immobile
                _alerte=BLOCKED;
                ptrTaskFollow->setEnabled(false); //on arrête la tâche de suivi de ligne
                motors.stop(); //on coupe les moteurs
                buzzer.buzz(750, 2, 600); //2 buzz moyens
                _countBlocked=0;
                break; //on sort du switch
              }
            } else {
              _countBlocked=0;
              _lastDeviation=newDeviation;
            }
          }
          break;
        case LOST:               
          if(ptrTaskFollow->isEnabled()){ //annulation de l'alerte
              ledJaune.setOn(false);
              _countLost=0;
              _alerte=NONE;
          } else { //on fait clignoter la led toutes les secondes
            _countLost++;
            if(_countLost>=25){
              ledJaune.swap();
              _countLost=0;
            }
          }
          break;
        case BLOCKED :
          if(ptrTaskFollow->isEnabled()){ //annulation de l'alerte
              ledJaune.setOn(false);
              _alerte=NONE;
          } else { //on maintient la led jaune allumée
              ledJaune.setOn(true);
          }
          break;
      }
   }
};
TaskGardian taskGardian;

#endif
//=====================
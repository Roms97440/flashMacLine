/* 
Programme : TestRoue
*/
#include "Scheduler.h"
//#include "Runner.h"
#include "task/RMonitor.h"
#include "Logger.h"
#include "sensor/Ina219.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Button.h"
#include "Pindef.h" //definitions des pins du robot
#include "actuator/Motor.h"


// Motor roueDroite(MV_RIGHT,true); //activation de la tâche
// Motor roueGauche(MV_LEFT);

SmothMotor roueDroite(MV_RIGHT, true); 
SmothMotor roueGauche(MV_LEFT,false, 3, 3); //la roue gauche semble 3% plus faible que la droite 

//Création des objets :
  //Les éléments de log
RMonitor rmonitor;            //pour l'affichage de la consommation des ressources (cpu/mémoire)
SensorINA219 capteurINA219;   //pour l'affichage des données du capteur de tension/courant

  //Les Led
Led ledRouge(PIN_LED_ROUGE);
Led ledJaune(PIN_LED_JAUNE);

  //Le buzzer
Buzzer buzzer(PIN_BUZZER);

  //Les boutons
class BtBleu : public Button {
  public :
    SETNAME("Bouton Bleu")         //on fixe le nom de cette tâche
    BtBleu() : Button(PIN_BT_BLEU){}
    void onPressed() override {
       ledRouge.setOn(true);
       //roueGauche.forward(100); 
       roueGauche.move(FORWARD, 100); 
    }
    void onReleased() override {
       //buzzer.bip();
       ledRouge.setOn(false);
       roueGauche.stop();
    }
};
BtBleu btBleu;

class BtGris : public Button {
  public :
    SETNAME("Bouton Gris")         //on fixe le nom de cette tâche
    BtGris() : Button(PIN_BT_GRIS){}
    void onPressed() override {
       ledJaune.setOn(true);
       //roueDroite.forward(100); 
       roueDroite.move(FORWARD, 100); 
    }
    void onReleased() override {
        //buzzer.buzz();
        ledJaune.setOn(false);
        roueDroite.stop();
    }    
};
BtGris btGris;
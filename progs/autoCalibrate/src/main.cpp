#include "Scheduler.h"
#include "task/RMonitor.h"  //inclus déjà "Logger.h"
//éléments du robot (commenter ceux non nécessaires):
#include "Pindef.h" //definitions des pins du robot
#include "sensor/Button.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Qtr-3RC.h"
#include "actuator/Motor.h"
#include "sensor/Ina219.h"

//Tache de base : log des ressources + log de la batterie et son avertisseur LedRouge
RMonitor rmonitor;            //pour l'affichage de la consommation des ressources (cpu/mémoire)
SensorINA219 capteurINA219; //affichage des données du capteur de tension/courant
LedBat ledBat(PIN_LED_ROUGE, capteurINA219); //-> led rouge == alerte batterie

Led ledJaune(PIN_LED_JAUNE);//pour signaler la calibration (clignote= lancement / fixe=calibration en cours)
SensorQTR_3RC capteur;

//Le buzzer
Buzzer buzzer(PIN_BUZZER);

//Configuration du moteur (faire des essais TestForward pour ajuster le delta des roues si le mouvement dévie trop)
SmothMotor _roueDroite(RIGHT,true); 
SmothMotor _roueGauche(LEFT,false); 
BiMotor motors(_roueDroite, _roueGauche);
constexpr uint8_t SPEED_FORWARD = 100; //constante de vitesse pour le TestForward
constexpr uint8_t SPEED_DANSE = 90;  //constante de vitesse pour la calibration automatique du capteur de ligne

/*
  => Tâche de calibration automatique du capteur de ligne.
Protocole :
- placer le robot au sol, perpendiculairement à la ligne, le capteur juste avant la ligne (sur du blanc)
- appuyer sur le bouton GRIS. La led jaune va clignoter pendant 5 secondes.
- la calibaration commence. La led jaune reste fixe durant toute la calibration.
  (le robot fait de mouvement avant/arrière pour passer plusieurs fois sur le capteur)
- au bout des 10 secondes : la led jaune s'eteind, le robot émet 3 bip, et la calibration est terminée.

NB: cette classe log aussi l'état du capteur de ligne toutes les secondes 
    (sauf durant les 10 secondes de lancement d'une phase de calibration)
*/
class AutoCalib : public Task { //la classe spécifique de cette tâche
  public :
    SETNAME("AutoCalib")
    enum StateDanse{NO_DANSE, DANSE_FORWARD, DANSE_BACKWARD};

  protected :
    uint8_t _count;//compteur de lancement, si !=0 la calibration est en cours de lancement
    StateDanse _danse;
    bool _pause; //pour intercaler une pause de 500ms entre chaque mouvement de danse

  public :
    AutoCalib() : Task(1000), _count(0), _danse(NO_DANSE), _pause(false){}
      //NB: on fixe une période de base de 1000ms pour les log d'état du capteur de ligne
    void run() override {
      if(_count>0){//phase de lancement de la calibration
        _count--;
        if(_count==0){
          ledJaune.setOn(true);
          capteur.calibrate();
          //Debut de la danse
          _danse=DANSE_FORWARD;
          _pause=false;
        } else {
          ledJaune.swap();
        }
      } else { //on log l'atat du cpateur de ligne
        switch(capteur.state()){
          case SensorQTR_3RC::NEED_CALIBRATE:
            LOG_INFO("LogQtr : capteur non calibré");
            break;
          case SensorQTR_3RC::CALIBRATION: //la calibration est en cours     
            if(_pause) { //on fait une pause durant cette demi-période
              _pause=false;
              motors.stop();
              LOG_INFO("LogQtr : ... Calibration en cours ...");
            } else { //on applique le mouvement de danse courant
              motors.move(_danse==DANSE_FORWARD ? FORWARD : BACKWARD, SPEED_DANSE);
              _danse=_danse==DANSE_BACKWARD ? DANSE_FORWARD: DANSE_BACKWARD;
              _pause=true;
            } 
            break;
          case SensorQTR_3RC::READY:
            if(_danse!=NO_DANSE){ //On vient de terminer une danse de calibration, on doit arrêter la danse 
              _danse=NO_DANSE;
              ledJaune.setOn(false);
              motors.stop();
              setPeriod(1000); //on restaure la periodicité à 1s
              buzzer.bip(700, 3, 500); //3 bip pour signaler la fin de la calibration
            } else {
              LOG_INFO("LogQtr : ** deviation =",capteur.deviation(),F("**     (v1 ="),capteur.values(0),F("v2 ="),capteur.values(1),F("v3 ="),capteur.values(2), F(")"));
            }
            break;
        }
      }
    }
    void launchCalibration(){//Lance la calibration dans un délai de 5 sec, 
        // mais si un lancement est en cours, ou que la calibration est en cours
        // l'action sera simplement d'annuler le lancement ou la calibration
      if(_danse!=NO_DANSE){ //une calibration est en cours, on l'annule
        _danse=NO_DANSE;
        capteur.cancelCalibration(); //on annule la calibration du capteur
        ledJaune.setOn(false);
        motors.stop();
        setPeriod(1000); //on restaure la periodicité à 1s
        buzzer.buzz(1500); //long buzz d'annulation
      } else if(_count>0){ //une phase de lancement est en cours, on l'annule
        _count=0;
        ledJaune.setOn(false);
        setPeriod(1000); //on restaure la periodicité à 1s
        buzzer.buzz(); //buzz court d'annulation
      } else { //vrai lancement de la calibration
        setPeriod(500); //on passe en demi-période (donc 500ms durant le lancement et la calibration)
        _count=10; // 10 * 500ms = 5s => lancement de la calibration dans 5 secondes
      }
    }
};
AutoCalib autoCalib; //activation de la tâche

  //Le bouton GRIS => Calibrage
class BtCalibrate : public Button {
  public :
    SETNAME("BtCalibrate")         //on fixe le nom de cette tâche
    BtCalibrate() : Button(PIN_BT_GRIS){}
    void onPressed() override {
      autoCalib.launchCalibration();
    }
};
BtCalibrate btCalibre;

class TestForward : public Task { //pour faire avancer le robot pendant 10 secondes.
 protected :
 uint8_t _count;
   //... données internes de cette tâche ...
 public :
   SETNAME("TestForward") //nom affiché dans le bilan de lancement de RMonitor
   TestForward() : Task(100), _count(0) {} //on fixe ici la rythmique 
       //-> l'activité va se déclencher toutes les PERIODE ms
   void run() override {
    if(_count!=0){
      _count--;
      if(_count==0) {
        motors.stop();
      }
    }
   }
   void start(uint8_t duree = 6) { //durre en seconde, 20 secondes maximum
    _count=duree<20 ? duree*10 : 200;
    motors.move(FORWARD,SPEED_FORWARD);
   }
};
TestForward testForward; //activation de la tâche (remplacer aussi `_name_`)

  //Le bouton BLEU => Test de ligne droite (6 secondes)
class BtGoForward : public Button { //Pour lancer la tâche TestForward
  public :
    SETNAME("BtGoForward")         //on fixe le nom de cette tâche
    BtGoForward() : Button(PIN_BT_BLEU){}
    void onPressed() override {
      testForward.start(6); //6 secondes en ligne droite
    }
};
BtGoForward btGoForward;
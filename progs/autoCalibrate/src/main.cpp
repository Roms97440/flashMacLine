// /!\ ATTENTION /!\ codage en cours...
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

//Tache de base : logRessource, logBatterie et avertisseurLedRouge
RMonitor rmonitor;            //pour l'affichage de la consommation des ressources (cpu/mémoire)
SensorINA219 capteurINA219; //affichage des données du capteur de tension/courant
LedBat ledBat(PIN_LED_ROUGE, capteurINA219); //-> led rouge == alerte batterie

Led ledJaune(PIN_LED_JAUNE);//pour signaler la calibration (clignote= lancement / fixe=calibration en cours)
SensorQTR_3RC capteur;

Motor roueDroite(PIN_M1_PWM,PIN_M1_DIR,true); //activation de la tâche
Motor roueGauche(PIN_M2_PWM,PIN_M2_DIR,false, 5, 5);
uint8_t speed=100; 
class AutoCalib : public Task { //la classe spécifique de cette tâche
  public :
    SETNAME("AutoCalib")
    enum StateDanse{NO, LEFT, RIGHT/*, BEGIN, END*/};

  protected :
    uint8_t _count;//compteur de lancement, si !=0 calibration en cours de lancement
    StateDanse _danse;
    bool _center;

  public :
    AutoCalib() : Task(1000), _count(0), _danse(NO), _center(true){ //on fixe ici la rythmique 
    }
    void danse(){
      if(_danse==LEFT){
              roueGauche.backward(speed,20,10);
              roueDroite.forward(speed,20,10);
      } else {
              roueGauche.forward(speed,20,10);
              roueDroite.backward(speed,20,10);
      }
    }
    void run() override {
      if(_count>0){//phase de lancement de la calibration
        _count--;
        ledJaune.swap();
        if(_count==0){
          setPeriod(500);
          capteur.calibrate();
          //Debut de la danse
          _danse=LEFT;
          _center=true;
        } 
      } else {
        switch(capteur.state()){
          case SensorQTR_3RC::NEED_CALIBRATE:
            ledJaune.setOn(false);
            LOG_INFO("LogQtr : capteur non calibré");
            break;
          case SensorQTR_3RC::CALIBRATION:
            ledJaune.setOn(true);
            if(_center) {
              _center=false;
            }else{
              _center=true;
              _danse=_danse==LEFT ? RIGHT : LEFT;
            } 
            danse();
            LOG_INFO("LogQtr : ... Calibration en cours ...");
            break;
          case SensorQTR_3RC::READY:
            ledJaune.setOn(false);
            if(_danse!=NO){
              //On doit arrêter la danse de calibration 
              if(_center){
                roueDroite.stop();
                roueGauche.stop();
                _danse=NO;
                setPeriod(1000);
              } else {
                _center=true;
                _danse=_danse==LEFT ? RIGHT : LEFT;
                danse();
              }
            }
            LOG_INFO("LogQtr : ** deviation =",capteur.deviation(),F("**     (v1 ="),capteur.values(0),F("v2 ="),capteur.values(1),F("v3 ="),capteur.values(2), F(")"));
            break;
        }
      }
    }
    void launchCalibration(){//Lance la calibration dans un délai de 5 sec
      setPeriod(500);
      _count=10; // 10 * 500ms = 5s
    }
};
AutoCalib autoCalib; //activation de la tâche

  //Les boutons
class BtCalibrate : public Button {
  public :
    SETNAME("Bouton Bleu")         //on fixe le nom de cette tâche
    BtCalibrate() : Button(PIN_BT_BLEU){}
    void onPressed() override {
      autoCalib.launchCalibration();
    }
};
BtCalibrate btBleu;



class Forward10 : public Task { //classe spécifique à une tâche
 protected :
 uint8_t _count;
   //... données internes de cette tâche ...
 public :
   SETNAME("EN AVANT !!!!!!!!!!!!!!!!!!!!!!!!") //nom affiché dans le bilan de lancement de RMonitor
   Forward10() : Task(1000), _count(0) {} //on fixe ici la rythmique 
       //-> l'activité va se déclencher toutes les PERIODE ms
   void run() override {
    if(_count!=0){
      _count--;
      if(_count==0) {
        roueGauche.stop();
        roueDroite.stop();
      }
    }
   }
   void start(uint8_t count = 10) {
    _count=count;
    roueDroite.forward(100,30,5);
    roueGauche.forward(100,30,5);
    
   }
};
Forward10 forward; //activation de la tâche (remplacer aussi `_name_`)



  //Les boutons
class BtForward : public Button {
  public :
    SETNAME("Bouton EN AVANT !!!")         //on fixe le nom de cette tâche
    BtForward() : Button(PIN_BT_GRIS){}
    void onPressed() override {
      forward.start(6);
    }
};
BtForward btForward;

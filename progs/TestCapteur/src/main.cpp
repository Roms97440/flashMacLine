#include "Scheduler.h"
//#include "Runner.h"
#include "task/RMonitor.h"
#include "Logger.h"
#include "sensor/Ina219.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Button.h"
#include "Pindef.h" //definitions des pins du robot
#include "sensor/Qtr-3RC.h"


Led ledJaune(PIN_LED_JAUNE);//pour signaler la calibration (clignote= lancement / fixe=calibration en cours)
SensorQTR_3RC capteur;

class LogQTR : public Task { //la classe spécifique de cette tâche
  protected :
    uint8_t _count;//compteur de lancement, si !=0 calibration en cours de lancement
  public :
    LogQTR() : Task(1000), _count(0){ //on fixe ici la rythmique 
    }
    
    void run() override {
      if(_count>0){//phase de lancement de la calibration
        _count--;
        ledJaune.swap();
        if(_count==0) capteur.calibrate();
      } else {
        switch(capteur.state()){
          case SensorQTR_3RC::NEED_CALIBRATE:
            ledJaune.setOn(false);
            LOG_INFO("LogQtr : capteur non calibré");
            break;
          case SensorQTR_3RC::CALIBRATION:
            ledJaune.setOn(true);
            LOG_INFO("LogQtr : ... Calibration en cours ...");
            break;
          case SensorQTR_3RC::READY:
            setPeriod(1000);
            ledJaune.setOn(false);
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
LogQTR logqtr; //activation de la tâche

  //Les boutons
class BtCalibrate : public Button {
  public :
    SETNAME("Bouton Bleu")         //on fixe le nom de cette tâche
    BtCalibrate() : Button(PIN_BT_BLEU){}
    void onPressed() override {
      logqtr.launchCalibration();
    }
};
BtCalibrate btBleu;
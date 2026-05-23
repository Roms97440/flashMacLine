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
#include "actuator/Motor.h"




Motor roueDroite(PIN_M1_PWM,PIN_M1_DIR,true); //activation de la tâche
Motor roueGauche(PIN_M2_PWM,PIN_M2_DIR);


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



class TestAlgo : public Task { //la classe spécifique de cette tâche
  protected :
    //... données internes de cette tâche ...
    float Kp = 1.5;//Proportionnel
    float Ki=0;//constante
    float Kd=0;//derivé
    long dt =1;
    long dtpred=1;
    float lastErreur=0;
    uint8_t BASE_SPEED = 90;
    uint8_t MAX_SPEED = 200;
    uint8_t GAIN = 20;
  public :
    SETNAME("Algo")
    TestAlgo() : Task(1000){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... valeurs de départ des données internes ...
    }
    int clamp(int value, int min, int max){
      return min(max(value,min),max);
    }

    void run() override {
      
      unsigned long dt = micros()-dtpred;
      
      //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
      int somme = capteur.values(0)+capteur.values(1)+capteur.values(2);
      int num=capteur.values(0)+capteur.values(2);
      float erreur = num/somme;
      
        
      // Terme Proportionnel : réaction immédiate à l'écart
      float P = Kp * erreur;
      float integral=0;
      
      // Terme Intégral : corrige les erreurs persistantes (biais)
      integral += erreur * (float)dt/1000000;
      integral = clamp(integral, -10, 10); // anti-windup
      float I = Ki * integral;//erreur passé => pour ajustement des moteur 

      // Terme Dérivé : anticipe et amortit les oscillations
      float D = Kd * (erreur - lastErreur) / dt;
      lastErreur = erreur;
      float correction = P + I + D;

      // Appliquer aux moteurs
      // int motorLeft  = clamp(BASE_SPEED + correction * GAIN, 0, MAX_SPEED);
      // int motorRight = clamp(BASE_SPEED - correction * GAIN, 0, MAX_SPEED);

      roueDroite.forward(clamp(BASE_SPEED + correction * GAIN, 0, MAX_SPEED));
      roueGauche.forward(clamp(BASE_SPEED - correction * GAIN, 0, MAX_SPEED));
      lastErreur=erreur;
      dtpred=micros();
    }
};
TestAlgo test; //activation de la tâche
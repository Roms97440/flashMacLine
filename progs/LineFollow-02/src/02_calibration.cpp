//Fichier source secondaire: 
#include "00_config.h"

/* ===> Tâche de calibration automatique du capteur de ligne, par mouvement avant-arrière

Protocole de calibration :
- placer le robot au sol, perpendiculairement à la ligne, le capteur juste avant la ligne (sur du blanc)
- appuyer sur le bouton GRIS. La led jaune va clignoter pendant 5 secondes.
- la calibaration commence. La led jaune reste fixe durant toute la calibration.
  (le robot fait de mouvement avant/arrière pour passer plusieurs fois sur le capteur)
- au bout des 10 secondes : la led jaune s'etteind, le robot émet 3 bip, et la calibration est terminée.

NB: - cette classe log aussi l'état du capteur de ligne toutes les secondes 
    (sauf durant les 10 secondes de lancement d'une phase de calibration)
    -> il devrait donc passer sur la ligne entre 10 et 12 fois.

    - pour annuler la calibration (ou son lancement) appuyez simplement à nouveau sur le bouton GRIS.
*/
/* #region(close) les #include */
#include "sensor/Button.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Qtr-3RC.h"
#include "actuator/Motor.h"
/* #endregion */

NAMED_LED(ledJaune, PIN_LED_JAUNE, "Led Jaune -> signal de calibration"); //pour signaler la calibration (clignote= lancement / fixe=calibration en cours)
// équivalent à :
// Led ledJaune(PIN_LED_JAUNE);

//Le capteur de ligne
SensorQTR_3RC capteur;

/* #region(close) les objets définis dans les autres fichiers */
extern Buzzer buzzer;
extern BiButton biButton;
extern BiMotor motors; //les moteurs sont configurés dans le fichier 01_main.cpp
/* #endregion */

// ==== constantes de réglage =============
constexpr uint8_t SPEED_DANSE = 110;  //constante de vitesse pour les mouvements de danse de la calibration automatique
// ====  ====  ====  ====  ====  ====  ==== 

class AutoCalib : public Task { //Tâche de gestion de la calibration et des logs du capteur de ligne
  public :
    SETNAME("AutoCalib")
/* #region(close) (attributs internes) */
    enum StateDanse{NO_DANSE, DANSE_FORWARD, DANSE_BACKWARD};
  protected :
    uint8_t _count;//compteur de lancement, si !=0 la calibration est en cours de lancement
    StateDanse _danse;
    bool _pause; //pour intercaler une pause de 500ms entre chaque mouvement de danse
/* #endregion */
  public : //API
    AutoCalib() : Task(1000), _count(0), _danse(NO_DANSE), _pause(false){}
      //NB: on fixe une période de base de 1000ms pour les log d'état du capteur de ligne
    bool launchCalibration(){//Lance la calibration dans un délai de 5 sec, 
        // mais si un lancement est en cours, ou que la calibration est en cours
        // l'action sera simplement d'annuler le lancement ou la calibration
      if(_danse!=NO_DANSE){ //une calibration est en cours, on l'annule
        _danse=NO_DANSE;
        capteur.cancelCalibration(); //on annule la calibration du capteur
        ledJaune.setOn(false);
        motors.stop();
        setPeriod(1000); //on restaure la periodicité à 1s
        buzzer.buzz(1500); //long buzz d'annulation
        return false;
      } else if(_count>0){ //une phase de lancement est en cours, on l'annule
        _count=0;
        ledJaune.setOn(false);
        setPeriod(1000); //on restaure la periodicité à 1s
        buzzer.buzz(); //buzz court d'annulation
        return false;
      } else { //vrai lancement de la calibration
        buzzer.bip(); // 1 bip pour cette action numéro 1
        setPeriod(500); //on passe en demi-période (donc 500ms durant le lancement et la calibration)
        _count=10; // 10 * 500ms = 5s => lancement de la calibration dans 5 secondes
        return true;
      }
    }
/* #region(close) (implémentations internes) */
    void run() override {
      if(_count>0){//phase de lancement de la calibration
        _count--;
        if(_count==0){
          ledJaune.setOn(true);
          capteur.calibrate();
          //Debut de la danse
          motors.setSmoth(10, 10); //on augmente la réactivité des moteurs 
              //pour qu'ils répondent mieux aux mouvements rapides avanr-arrière
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
              motors.stop();//motors.stop(false, true);
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
              motors.resetSmoth(); //on restaure la réactivité des moteurs
              ledJaune.setOn(false);
              motors.stop();
              setPeriod(1000); //on restaure la periodicité à 1s
              buzzer.buzz(800); //buzz pour signaler la fin de la calibration
              biButton.closeAction(BiButton::BT1);
            } else {
              LOG_INFO("LogQtr : ** deviation =",capteur.deviation(),F("**     (v1 ="),capteur.values(0),F("v2 ="),capteur.values(1),F("v3 ="),capteur.values(2), F(")"));
            }
            break;
        }
      }
    }
/* #endregion */
};
AutoCalib autoCalib; //activation de la tâche

SET_ACTION(setActionAutoCalib, biButton, BT1, [](bool launch, uint8_t bt){ //action sur le bouton Gris
  return autoCalib.launchCalibration();
});

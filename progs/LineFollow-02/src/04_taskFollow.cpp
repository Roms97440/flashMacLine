//Fichier source secondaire: 
#include "00_config.h"
//  ===> Tâche principale pour le suivi de ligne !!!! 

/* #region(close) les #include */
#include "sensor/Button.h"
#include "actuator/Motor.h"
#include "actuator/Buzzer.h"
#include "sensor/Qtr-3RC.h"
/* #endregion */

/* #region(close) les objets définis dans les autres fichiers */
extern BiButton biButton;
extern Buzzer buzzer;
extern SensorQTR_3RC capteur;
extern BiMotor motors;
/* #endregion */

// ====  Constante de réglage (à ajuster empiriquement)  ====
constexpr uint8_t maxSpeed = 90; //vittesse maximum des roues
constexpr int vBase = 150; //150
constexpr float Kp = 0.2;//.20 correction instantannée
constexpr float Kd=0.3;//derivé.15 smoothing correction
constexpr float gain = 0.65;//.65 force de correction par rapport à la vitesse de base
// ====  ====  ====  ====  ====  ====  ====  ====  ====  ====

// ***  Variables exploitées par l'algo de pilotage   ***
long dt =1;
long dtpred=1;
float lastErreur=0;
int16_t erreur=0;
// ***  ***  ***  ***  ***  ***  ***  ***  ***  ***  ***

/* #region(close) Code fixe avant l'algo de pilotage */
class TaskFollow : public Task {
 protected :
   //... données internes de cette tâche ...
 public :
   SETNAME("TaskFollow") //nom affiché dans le bilan de lancement de RMonitor
   TaskFollow() : Task(capteur, false) {} //cette tâche va suivre la rythmique du capteur de ligne 
      //---> elle sera tjs exécutée après la tâche de relevé de mesure, soit tutes les 20ms
   void run() override {  
/* #endregion */

//============> Alog de pilotage  <================

     // 1. Récupération de l'erreur (la déviation)
     lastErreur = erreur;
     erreur = capteur.deviation();
     
     dtpred=dt;
     dt=micros();

     // 2. Calcul de la correction Proportionnelle
    float D = Kd * ((erreur - lastErreur) / (dt-dtpred));
    int correction = erreur * Kp + D; 
     
     // 3. Calcul des vitesses théoriques (non bridées, peuvent être négatives)
     int vGauche = vBase - correction*gain;
     int vDroite = vBase + correction*gain;
     
     // 4. Déduction logique du sens pour chaque roue (valeur négative -> changement de sens + passage de la valeur en positive)
     bool sensGauche, sensDroite;
     if(vGauche >= 0) sensGauche=FORWARD;
     else {
        sensGauche=BACKWARD;
        vGauche=-vGauche;
     }
     if(vDroite >= 0) sensDroite=FORWARD;
     else {
        sensDroite=BACKWARD;
        vDroite=-vDroite;
     }
     
     // 5. Extraction de la puissance absolue et bridage matériel (0-maxSpeed)
     uint8_t pwmGauche = min(vGauche, maxSpeed);
     uint8_t pwmDroite = min(vDroite, maxSpeed);
     
     // 6. Envoi des consignes
     //ajouter true en 3e param de move pour désactiver le smoothing
     motors.moveLR(sensGauche, pwmGauche, sensDroite, pwmDroite);

//============  ============  ============  ============

/* #region(close) Code fixe après l'algo de pilotage */     
   }

   void ajustSpeed(int16_t erreur){ //force ke le robot à ralentir si l'erreur est trop importante
     // int16_t abs_erreur = erreur>0 ? erreur : -erreur;
     // if(abbs_erreur>750)
   }

   void start() { 
        buzzer.bip(400, 2, 200);
        setEnabled(true);
   }
   void stop(){
        buzzer.buzz(800);
        motors.stop();
        setEnabled(false);
   }
};
TaskFollow taskFollow;
Task* ptrTaskFollow = &taskFollow; //pour la tâche Gardian

SET_ACTION(setActionTaskFollow, biButton, BT2, [](bool launch, uint8_t bt){ //action sur l'activation du bouton bleu
     if(launch) {
      taskFollow.start();
      return true;
     } else {
      taskFollow.stop();
      return false;
     }
});
/* #endregion */


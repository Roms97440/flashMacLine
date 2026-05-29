//Fichier source secondaire: 
#include "00_config.h"

/*  ===> Tâche principale pour le suivi de ligne !!!! 

  -> cette tâche fait (normalement) avancer le robot le long de la ligne noire ;)
  
*/
/* #region(close) les #include */
#include "sensor/Button.h"
#include "actuator/Motor.h"
#include "actuator/Buzzer.h"
#include "sensor/Qtr-3RC.h"
/* #endregion */

extern BiButton biButton;
extern Buzzer buzzer;
extern BiMotor motors; //les moteurs sont configurés dans le fichier 01_main.cpp

extern SensorQTR_3RC capteur;
extern SmothMotor roueDroite; 
extern SmothMotor roueGauche; 
extern BiMotor motors;

// Variables de réglage (à ajuster empiriquement)
constexpr int vBase = 150; //150
constexpr float Kp = 0.2;//.20 correction insrtantannee
constexpr float Kd=0.3;//derivé.15 smoothing correction
constexpr float gain = 0.65;//.65 force de correction par rapport à la vitesse de base
long dt =1;
long dtpred=1;
float lastErreur=0;
int16_t erreur=0;
class TaskFollow : public Task { //pour faire avancer le robot pendant 10 secondes.
 protected :
   //... données internes de cette tâche ...
 public :
   SETNAME("TaskFollow") //nom affiché dans le bilan de lancement de RMonitor
   TaskFollow() : Task(20, false) {} //on fixe ici la rythmique 
       //-> l'activité va se déclencher toutes les PERIODE ms
   void run() override {  

     // 2. Récupération de l'erreur
     lastErreur = erreur;
     erreur = capteur.deviation();
     
     dtpred=dt;
     dt=micros();
     // 3. Calcul de la correction Proportionnelle
     
    float D = Kd * ((erreur - lastErreur) / (dt-dtpred));
    int correction = erreur * Kp + D; 
     
     // 4. Calcul des vitesses théoriques (non bridées, peuvent être négatives)
     int vGauche = vBase - correction*gain;
     int vDroite = vBase + correction*gain;
     
     // 5. Déduction logique du sens pour chaque roue
     bool sensGauche = (vGauche >= 0) ? FORWARD : BACKWARD;
     bool sensDroite = (vDroite >= 0) ? FORWARD : BACKWARD;
     
     // 6. Extraction de la puissance absolue et bridage matériel (0-255)
     uint8_t pwmGauche = constrain(abs(vGauche), 0, 90); //255);
     uint8_t pwmDroite = constrain(abs(vDroite), 0, 90); //255);
     

     // 7. Envoi des consignes
     //ajouter true en 3e param de move pour désactiver le smoothing
     roueGauche.move(sensGauche, pwmGauche);
     roueDroite.move(sensDroite, pwmDroite);
   }
   void ajustSpeed(int16_t erreur){ //force ke le robot à ralentir si l'erreur est trop importante
     // int16_t abs_erreur = erreur>0 ? erreur : -erreur;
     // if(abbs_erreur>750) 

   }
   void start() { 
        buzzer.notif(200, 3, 200);
        setEnabled(true);
   }
   void stop(){
        buzzer.buzz(800);
        motors.stop();
        setEnabled(false);
   }
};
TaskFollow taskFollow;

SET_ACTION(setActionTaskFollow, biButton, BT2, []{ //action sur l'activation du bouton bleu
    
     if(taskFollow.isEnabled()) taskFollow.stop();
     else taskFollow.start();
});


/* #region la tâche ajustTiming */
#ifdef LIGH_PERIOD
//Allégement des périodes des tâches principales (Capteur et TaskFollow)
void ajustTiming(){
  capteur.setPeriod(30);
  taskFollow.setPeriod(40);
}
NeedInit initAjustTiming(ajustTiming,true);
#endif
/* #endregion */

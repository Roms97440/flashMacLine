//Fichier principal (le seul en inclure "Scheduler.h", les autres doivent inclure "Runner.h")
#include "Scheduler.h"

/* ===> Ce code de base fixe les éléments essentiels du Robot.

-> à utiliser comme fichier source principal dans chacun des programmes

Il met en place :
 - le log des ressources (via RMonitor)
 - log de l'état de la batterie et son avertisseur LedRouge

*/

#include "task/RMonitor.h"  //inclus déjà "Logger.h"
#include "Pindef.h"         //definitions des pins du robot
#include "sensor/Ina219.h"
#include "actuator/Motor.h"
#include "actuator/Buzzer.h"

//Moniteur de ressource RAM/CPU
RMonitor rmonitor;            //pour l'affichage de la consommation des ressources (cpu/mémoire)
  //--> à commenter pour la phase d'upload final (pour alléger le code embarqué)

//Gestion de la batterie
SensorINA219 capteurINA219;   //affichage des données du capteur de tension/courant
LED_BAT(ledBat, PIN_LED_ROUGE, capteurINA219, "Led Rouge -> batterie");
  //cette macro est un équivalent plus pratique de :
  // LedBat ledBat(PIN_LED_ROUGE, capteurINA219); //-> led rouge == alerte batterie


//Configuration des moteurs (faire des essais TestForward pour ajuster le delta des roues si le mouvement dévie trop)
SmothMotor _roueDroite(MV_RIGHT,true); 
SmothMotor _roueGauche(MV_LEFT,false,3,3); //la roue gauche semble 3% plus faible que la droite 
BiMotor motors(_roueDroite, _roueGauche);

//Le buzzer
Buzzer buzzer(PIN_BUZZER);
/* 
Programme : TestAccessoire

Description de ce programme test :
Ce programme minimaliste permet de tester le bon fonctionnement des différents 
composants électroniques du robot :
 - ses 2 leds : rouge et jaune (la verte est connectée directement à l'alim).
 - ses 2 boutons : bleu et gris.
 - son buzzer.
 - son capteur de tension/courant (ina219).

Voici ce que fait ce programme une fois chargé dans l'Arduino : 
  - un appui sur le bouton bleu bascule l'état ON/OFF de la led rouge
  - un appui sur le bouton gris bascule l'état ON/OFF de la led verte
  - le relachement du bouton bleu produit un bip
  - le relachement du bouton gris produit un buzz (son grave)
  - quand il connecté à l'USB, les logs suivant sont réalisés toutes les 2 secondes :
    -> la consommation mémoire (via RMonitor)
    -> les niveaux de tension (V), de courant (A), la consommation (mW), 
        et le nivau de batterie (%) (via le capteur ina219)

Le code de ce programme d'exemple illustre également l'utilisation de la 
librairie Scheduler dans une syntaxe avancée (à base de classe).

NB: si la console affiche une valeur de tension dans 4-5v c'est que vous avez oublié de
mettre le robot sur ON (il fonctionne alors avec la charge du câble USB et non pas sur les accus).
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
       ledRouge.swap(); 
    }
    void onReleased() override {
       buzzer.bip();
    }
};
BtBleu btBleu;

class BtGris : public Button {
  public :
    SETNAME("Bouton Gris")         //on fixe le nom de cette tâche
    BtGris() : Button(PIN_BT_GRIS){}
    void onPressed() override {
       ledJaune.swap(); 
    }
    void onReleased() override {
        buzzer.buzz();
    }    
};
BtGris btGris;

//  void setup() {
//     Runner.init();  //initialisation du moteur d'exécution scheduler
// }

//  void loop() {
//     Runner.execute(); //traitement des tâches
// }
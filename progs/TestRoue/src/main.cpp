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



/*

pilotage des roues (Broches D4 à D7)

La direction (Broches D4 et D7) : Un simple état HIGH (avant) ou LOW (arrière).

La vitesse (Broches D5 et D6) : Un signal PWM de 0 (arrêt) à 255 (vitesse maximale).

Voici l'attribution matérielle figée sur le circuit imprimé du shield :

Moteur 1 (généralement Gauche) : Direction = D4 | Vitesse = D5

Moteur 2 (généralement Droit) : Direction = D7 | Vitesse = D6

*/

class Roue : public Task { //la classe spécifique de cette tâche
  protected :
    uint8_t _pinPWM,_pinDIR;
    bool _inversed;//à true si avant/arriere est inversé pour cette roue
    //... données internes de cette tâche ...
  public :
    Roue(uint8_t pinPWM, uint8_t pinDIR, bool inversed=false) : Task(1000,false), _pinPWM(pinPWM), _pinDIR(pinDIR), _inversed(inversed){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... valeurs de départ des données internes ...
    }
    void init() override {
      pinMode(_pinPWM, OUTPUT);
      pinMode(_pinDIR, OUTPUT);
        
    }
    void run() override {
        //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
        // MAIS seulement toutes les PERIODE millisecondes 
    }
    void forward(uint8_t speed){// Vitesse de 0 à 255

      digitalWrite(_pinDIR, _inversed ? LOW : HIGH);
      analogWrite(_pinPWM, speed);   
    }

    void backward(uint8_t speed){// Vitesse de 0 à 255
      digitalWrite(_pinDIR, _inversed ? HIGH : LOW);
      analogWrite(_pinPWM, speed);   
    }

    void stop(){// Vitesse de 0 à 255
      analogWrite(_pinPWM, 0);   
    }
};
Roue roueDroite(PIN_M1_PWM,PIN_M1_DIR,true); //activation de la tâche
Roue roueGauche(PIN_M2_PWM,PIN_M2_DIR);



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
       roueGauche.forward(100); 
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
       roueDroite.forward(100); 
    }
    void onReleased() override {
        //buzzer.buzz();
        ledJaune.setOn(false);
        roueDroite.stop();
    }    
};
BtGris btGris;

//  void setup() {
//     Runner.init();  //initialisation du moteur d'exécution scheduler
// }

//  void loop() {
//     Runner.execute(); //traitement des tâches
// }
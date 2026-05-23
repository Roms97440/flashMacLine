#pragma once

#include "Runner.h"


/*

pilotage des roues (Broches D4 à D7)

La direction (Broches D4 et D7) : Un simple état HIGH (avant) ou LOW (arrière).

La vitesse (Broches D5 et D6) : Un signal PWM de 0 (arrêt) à 255 (vitesse maximale).

Voici l'attribution matérielle figée sur le circuit imprimé du shield :

Moteur 1 (généralement Gauche) : Direction = D4 | Vitesse = D5

Moteur 2 (généralement Droit) : Direction = D7 | Vitesse = D6

*/

class Motor : public Task { //la classe spécifique de cette tâche
  protected :
    uint8_t _pinPWM,_pinDIR;
    bool _inversed;//à true si avant/arriere est inversé pour cette roue
    //... données internes de cette tâche ...
  public :
  SETNAME("Motor")
    Motor(uint8_t pinPWM, uint8_t pinDIR, bool inversed=false) : Task(1000,false), _pinPWM(pinPWM), _pinDIR(pinDIR), _inversed(inversed){ //on fixe ici la rythmique 
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
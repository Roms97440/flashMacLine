#pragma once

#include "Runner.h"
#include "Logger.h"

/*

pilotage des roues (Broches D4 à D7)

La direction (Broches D4 et D7) : Un simple état HIGH (avant) ou LOW (arrière).

La vitesse (Broches D5 et D6) : Un signal PWM de 0 (arrêt) à 255 (vitesse maximale).

Voici l'attribution matérielle figée sur le circuit imprimé du shield :

Moteur 1 (généralement Gauche) : Direction = D4 | Vitesse = D5

Moteur 2 (généralement Droit) : Direction = D7 | Vitesse = D6

*/
uint8_t safe_float_to_uint8(float f) {
    // 1. On arrondit à l'entier le plus proche (utilise le round standard de math.h inclus dans Arduino)
    float rounded = round(f);
    
    // 2. On limite la valeur entre 0 et 255 avec constrain() (propre à Arduino)
    float clamped = constrain(rounded, 0.0f, 255.0f);
    
    // 3. Conversion sécurisée
    return static_cast<uint8_t>(clamped);
}

class Motor : public Task { //la classe spécifique de cette tâche
  protected :
    uint8_t _pinPWM,_pinDIR;
    bool _inversed;//à true si avant/arriere est inversé pour cette roue
    uint8_t _smoothStep;
    uint8_t _smoothTarget;
    uint8_t _curSpeed;
    int8_t _deltaF , _deltaB;//coefficient de puissance de correction des moteurs en pourcentage [-100,100]
    bool _forward; //à true si on est en marche avant
    //... données internes de cette tâche ...
  public :
  SETNAME("Motor")
    Motor(uint8_t pinPWM, uint8_t pinDIR, bool inversed=false, int8_t deltaF=0 , int8_t deltaB=0) : Task(1000,false), _pinPWM(pinPWM), _pinDIR(pinDIR), _inversed(inversed), _smoothStep(0), _smoothTarget(0), _curSpeed(0), _deltaF(deltaF), _deltaB(deltaB), _forward(true){ //on fixe ici la rythmique 
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
          _curSpeed+=_smoothStep;
          if(_curSpeed>=_smoothTarget){
            _curSpeed=_smoothTarget;
            setEnabled(false);
          }
          _applySpeed();
    }

    void forward(uint8_t speed,uint16_t stepTime=0,uint8_t stepSpeed=10){// Vitesse de 0 à 255
      _forward=true;
      if(stepTime==0){
        _curSpeed=speed;
      } else {   
        _smoothStep=stepSpeed;
        _smoothTarget=speed;
        _curSpeed=50;
        setPeriod(stepTime);
        setEnabled(true);
      }
      _applySpeed();
    }

    void backward(uint8_t speed,uint16_t stepTime=0,uint8_t stepSpeed=10){// Vitesse de 0 à 255
      _forward=false;
      if(stepTime==0){
        _curSpeed=speed;  
      } else {   
        _smoothStep=stepSpeed;
        _smoothTarget=speed;
        _curSpeed=50;
        setPeriod(stepTime);
        setEnabled(true);
      }
      _applySpeed();
    }

    void stop(){// Vitesse de 0 à 255
      analogWrite(_pinPWM, 0);   
      setEnabled(false);
    }
    private: 
    void _applySpeed(){
       int8_t delta = 0;
      if(_forward) {
        digitalWrite(_pinDIR, _inversed ? LOW : HIGH);
        delta = _deltaF;
      } else {
        digitalWrite(_pinDIR, _inversed ? HIGH : LOW);
        delta = _deltaB;
      }
      if(delta!=0){
        float speed=_curSpeed*(1.0+delta/100.0);
        analogWrite(_pinPWM, safe_float_to_uint8(speed));
        //LOG_INFO("toto :",_inversed,safe_float_to_uint8(speed), delta);
      } else {
        analogWrite(_pinPWM, _curSpeed);
      }
    }
};
#pragma once

#include "Runner.h"
#include "Pindef.h"

class Motor : public NeedInit { //la classe de gestion simplifiée des moteur
 /* #region Attributs internes */
  protected :
    //configuration de base
    uint8_t _pinPWM,_pinDIR;
    bool _inversed; //à true si avant/arriere est inversé pour ce moteur (dépend de l'orientation du montage)
 /* #endregion */
  public :
    SETNAME("Motor")
    Motor(bool side, bool inversed=false) : NeedInit(), 
      _pinPWM(side ? PIN_M1_PWM : PIN_M2_PWM), _pinDIR(side ? PIN_M1_DIR : PIN_M2_DIR), _inversed(inversed) //Configuration de base
    {}
    Motor(uint8_t pinPWM, uint8_t pinDIR, bool inversed=false) : NeedInit(), 
      _pinPWM(pinPWM), _pinDIR(pinDIR), _inversed(inversed) //Configuration de base
    {}
    void init() override {
      pinMode(_pinPWM, OUTPUT);
      pinMode(_pinDIR, OUTPUT);
    }
    void forward(uint8_t speed){// Vitesse de 0 à 255
      digitalWrite(_pinDIR, _inversed ? LOW : HIGH);
      analogWrite(_pinPWM, speed); 
    }

    void backward(uint8_t speed){// Vitesse de 0 à 255
      digitalWrite(_pinDIR, _inversed ? HIGH : LOW);
      analogWrite(_pinPWM, speed);
    }
    void stop(){
      analogWrite(_pinPWM, 0);
    }
};

// /!\ ATTENTION /!\ codage en cours...
class SmothMotor : public Task { //moteur à correction de puissance et changement de vitesse progressif (smoth)
 /* #region Attributs internes */
  protected :
    //configuration de base
    uint8_t _pinPWM,_pinDIR;
    bool _inversed; //à true si avant/arriere est inversé pour ce moteur (dépend de l'orientation du montage)
    //compensation corrective de la force du moteur (pour l'équilibrage des roues)
      //-> dans le constructeur : ces valeurs sont exprimées en pourcentage [-100,100] (type int16_t)
      //-> en attribut (ici) : elles sont traduites en coefficient multiplicateur [0;2] pour optimiser les calculs
    float _deltaF; // compensation en marche avant
    float _deltaB; // compensation en marche arrière
    //gestion du `soft speed` : pour les commandes de mouvement non immédiate, le changement de vitesse sera effectué
    // progressivement en utilisant les paramètres ci-dessous
    bool _curSens;      // FORWARD / BACKWARD
    bool _targetSens;   // FORWARD / BACKWARD
    uint8_t _curSpeed;  //vitesse actuelle appliquée au moteur (avant compensation delta)
    uint8_t _targetSpeed; //vitesse cible
    uint8_t _step;  //pas d'incrément 
      //NB: la périodicié du pas se règle via setPériod(...) elle est de 20ms 
      // => pour un pas de 5 (valeurs par défaut) cela donne : 0 à 200 en 800ms

 /* #endregion */
  public :
    SETNAME("Motor")
    SmothMotor(bool side, bool inversed=false, int8_t deltaF=0 , int8_t deltaB=0) 
     : Task(20,false), 
     _pinPWM(side ? PIN_M1_PWM : PIN_M2_PWM), _pinDIR(side ? PIN_M1_DIR : PIN_M2_DIR), _inversed(inversed), //Configuration de base
     _curSens(FORWARD), _targetSens(FORWARD), _curSpeed(0), _targetSpeed(0), _step(5) //configuration du `soft speed`
    {   //pré-calcul du coéfficient de compensation de force (pour l'équilibrage des roues)
        _deltaF = deltaF == 0 ? 0 : (1.0+deltaF/100.0);
        _deltaB = _deltaB == 0 ? 0 : (1.0+deltaB/100.0);
    }
    void init() override {
      pinMode(_pinPWM, OUTPUT);
      pinMode(_pinDIR, OUTPUT);
    }
    void run() override { //traitement `soft speed`
      if (_curSpeed == _targetSpeed && _curSens == _targetSens) return; // RAS

      if(_curSens != _targetSens){ //on doit ramener _curSpeed à 

      }
        // // Logique de rapprochement progressif (Soft Start / Soft Stop)
        // if (_currentP < _targetP) {
        //     _currentP += _stepSize;
        //     if (_currentP > _targetP) _currentP = _targetP; // Évite de dépasser la cible
        // } else {
        //     _currentP -= _stepSize;
        //     if (_currentP < _targetP) _currentP = _targetP;
        // }

        // Application au Shield
//        analogWrite(_pinPWM, _currentP);
          // _applySpeed();
    }

    void forward(uint8_t speed,uint16_t stepTime=0,uint8_t stepSpeed=10){// Vitesse de 0 à 255
      // _forward=true;
      // if(stepTime==0){
      //   _curSpeed=speed;
      // } else {   
      //   // _smoothStep=stepSpeed;
      //   // _smoothTarget=speed;
      //   // _curSpeed=50;
      //   // setPeriod(stepTime);
      //   // setEnabled(true);
      // }
      // _applySpeed();
    }

    void backward(uint8_t speed,uint16_t stepTime=0,uint8_t stepSpeed=10){// Vitesse de 0 à 255
      // _forward=false;
      // if(stepTime==0){
      //   _curSpeed=speed;  
      // } else {   
      //   // _smoothStep=stepSpeed;
      //   // _smoothTarget=speed;
      //   // _curSpeed=50;
      //   // setPeriod(stepTime);
      //   // setEnabled(true);
      // }
      _applySpeed();
    }

    void stop(){// Vitesse de 0 à 255
      analogWrite(_pinPWM, 0);   
      setEnabled(false);
    }
    private: 
    void _applySpeed(){
      //  int8_t delta = 0;
      // if(_forward) {
      //   digitalWrite(_pinDIR, _inversed ? LOW : HIGH);
      //   delta = _deltaF;
      // } else {
      //   digitalWrite(_pinDIR, _inversed ? HIGH : LOW);
      //   delta = _deltaB;
      // }
      // if(delta!=0){
      //   float speed=_curSpeed*(1.0+delta/100.0);
      //   analogWrite(_pinPWM, safe_float_to_uint8(speed));
      //   //LOG_INFO("toto :",_inversed,safe_float_to_uint8(speed), delta);
      // } else {
      //   analogWrite(_pinPWM, _curSpeed);
      // }
    }
};
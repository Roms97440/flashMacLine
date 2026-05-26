#pragma once

#include "Runner.h"
#include "Pindef.h"

class Motor : public NeedInit { //la classe de gestion simplifiée des moteur
/* #region(collapsed) Attributs internes */
  protected :
    //configuration de base
    uint8_t _pinPWM,_pinDIR;
    bool _inversed; //à true si avant/arriere est inversé pour ce moteur (dépend de l'orientation du montage)
/* #endregion */
  
  public : //API
    SETNAME("Motor")
    Motor(bool side, bool inversed=false) : NeedInit(), 
      _pinPWM(side ? PIN_M1_PWM : PIN_M2_PWM), _pinDIR(side ? PIN_M1_DIR : PIN_M2_DIR), _inversed(inversed) //Configuration de base
    {}
    Motor(uint8_t pinPWM, uint8_t pinDIR, bool inversed=false) : NeedInit(), 
      _pinPWM(pinPWM), _pinDIR(pinDIR), _inversed(inversed) //Configuration de base
    {}
    
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

/* #region(collapsed) Méthodes internes */ 
    void init() override {
      pinMode(_pinPWM, OUTPUT);
      pinMode(_pinDIR, OUTPUT);
    }
/* #endregion */    
};

class SmothMotor : public Task { //moteur à correction de puissance et changement de vitesse progressif (`soft speed`)
/* #region(collapsed) Attributs internes */
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

  public : // API
    SETNAME("SmothMotor")
    SmothMotor(bool side, bool inversed=false, int8_t deltaF=0 , int8_t deltaB=0) 
     : Task(20, true, true), //période de 20ms + isochronisme pour le `soft speed`
     _pinPWM(side ? PIN_M1_PWM : PIN_M2_PWM), _pinDIR(side ? PIN_M1_DIR : PIN_M2_DIR), _inversed(inversed), //Configuration de base
     _curSens(FORWARD), _targetSens(FORWARD), _curSpeed(0), _targetSpeed(0), _step(5) //configuration du `soft speed`
    {   //pré-calcul du coéfficient de compensation de force (pour l'équilibrage des roues)
        _deltaF = deltaF == 0 ? 0 : (1.0+deltaF/100.0);
        _deltaB = deltaB == 0 ? 0 : (1.0+deltaB/100.0);
    }
    void setSmoth(uint8_t step /*max 30*/, uint32_t period=0){ //re-configuration du smothing
      _step = min(step, (uint8_t) 30);
      if(period!=0) setPeriod(period);
    }

    void move(bool sens, uint8_t speed,bool immediate=false){// Vitesse de 0 à 255
      _targetSens=sens;
      _targetSpeed=speed;

      if(immediate){
        _curSens = sens;
        digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW); 
        _curSpeed=speed;
        _applySpeed();
      }  
    }
    void stop(bool immediate=false, bool reverseSens=false){//arrêt des moteurs
      _targetSpeed = 0;
      if(reverseSens) _targetSens=!_curSens;
      if(immediate){
        _curSpeed=0;
        analogWrite(_pinPWM, 0);
      }  
    }

/* #region(collapsed) Méthodes internes */  
  public :   
    void init() override {
      pinMode(_pinPWM, OUTPUT);
      pinMode(_pinDIR, OUTPUT);
      digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW);  // Bascule matérielle
    }
    void run() override { //traitement `soft speed`
      if (_curSpeed == _targetSpeed && _curSens == _targetSens) return; // RAS

      if(_curSens != _targetSens){ //on doit ramener _curSpeed à une valeur < 30 pour changer le sens
        if (_curSpeed <= 30){ //on est suffisament ralenti pour faire l'inversion du sens
            _curSpeed = 0;              // On force l'arrêt électrique
            analogWrite(_pinPWM, 0);    // Application immédiate pour soulager le Shield
            
            _curSens = _targetSens;       // On valide le nouveau sens
            digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW); // Bascule matérielle
        } else {
          _curSpeed -= _step; // On freine
        }
      } else { //ici _curSpeed != _targetSpeed , on doit converger vers _targetSpeed
          if (_curSpeed < _targetSpeed) {
              if(_curSpeed<220) { //pour éviter le débordement uint8_t sur le +_step
                _curSpeed += _step;
                if (_curSpeed > _targetSpeed) _curSpeed = _targetSpeed;
              } else _curSpeed = _targetSpeed;
          } else { // ici : _curSpeed > _targetSpeed
              if(_curSpeed>30) { //pour éviter le débordement uint8_t sur le -_step
                _curSpeed -= _step;
                if (_curSpeed < _targetSpeed) _curSpeed = _targetSpeed;
              } else _curSpeed = _targetSpeed;
          }
          //Application de la puissance
          _applySpeed();
      }
    }
  private:
    void _applySpeed(){
      float delta = _curSens ? _deltaF : _deltaB;
      if(_curSpeed!=0 && delta!=0){
        float speed=_curSpeed*delta;
        analogWrite(_pinPWM, safe_float_to_uint8(speed));
      } else {
        analogWrite(_pinPWM, _curSpeed);
      }
    }
/* #endregion */    
};

class BiMotor : public NeedInit { //la classe de gestion simplifiée des 2 moteurs en même temps
/* #region(collapsed) Attributs internes */
  protected :
    SmothMotor* _motorR;
    SmothMotor* _motorL;
/* #endregion */
  
  public : //API
    SETNAME("BiMotor")
    BiMotor(SmothMotor& motorR, SmothMotor& motorL) : NeedInit(), 
      _motorR(&motorR), _motorL(&motorL){}
    
    void rotate(bool direction, uint8_t speed, bool immediate=false){// direction = RIGHT ou LEFT
      _motorR->move(direction, speed, immediate);
      _motorL->move(!direction, speed, immediate);
    }
    void move(bool sens, uint8_t speed,bool immediate=false){// Vitesse de 0 à 255
      _motorR->move(sens, speed, immediate);
      _motorL->move(sens, speed, immediate);
    }
    void stop(bool immediate=false, bool reverseSens=false){//arrêt des moteurs
      _motorR->stop(immediate, reverseSens);
      _motorL->stop(immediate, reverseSens); 
    }
};
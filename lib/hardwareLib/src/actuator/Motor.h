#pragma once

#include "Runner.h"
#include "Pindef.h"

enum TypeMotor { BASIC, SMOTH };

class AbstractMotor { //Classe de base d'un moteur
/* #region(close) Attributs internes */ 
 protected :
    uint8_t _pinPWM,_pinDIR;
    bool _inversed; //à true si avant/arriere est inversé pour ce moteur (dépend de l'orientation du montage)
 public:
  virtual ~AbstractMotor() = default;      
/* #endregion */ 
public: //API
  const TypeMotor type;
  AbstractMotor(TypeMotor type, bool side, bool inversed=false) :
    _pinPWM(side ? PIN_M1_PWM : PIN_M2_PWM), _pinDIR(side ? PIN_M1_DIR : PIN_M2_DIR), _inversed(inversed), type(type) {}
  
  //Méthodes à masquer :
  virtual void forward(uint8_t speed, bool immediate=false) = 0;
  virtual void backward(uint8_t speed, bool immediate=false) = 0;
  virtual void move(bool sens, uint8_t speed,bool immediate=false) = 0;
  virtual void stop(bool immediate=false, bool reverseSens=false) = 0;

/* #region(close) Méthodes internes */ 
/* A utiliser ainsi dans les classes filles de NeedInit ou Task
  void init() override { AbstractMotor::init(); }
*/
  void init() { 
    pinMode(_pinPWM, OUTPUT);
    pinMode(_pinDIR, OUTPUT);
  }
/* #endregion */   
};

class Motor : public AbstractMotor, public NeedInit { //Moteur brute (sans raffinement)
  public : //API
    SETNAME("Motor")
    Motor(bool side, bool inversed=false) : AbstractMotor(TypeMotor::BASIC, side, inversed), NeedInit(){}
    void init() override { AbstractMotor::init(); }

    void forward(uint8_t speed, bool immediate=false) override { //immediate ignorée
      digitalWrite(_pinDIR, _inversed ? LOW : HIGH);
      analogWrite(_pinPWM, speed); 
    }
    void backward(uint8_t speed, bool immediate=false) override { //immediate ignorée
      digitalWrite(_pinDIR, _inversed ? HIGH : LOW);
      analogWrite(_pinPWM, speed);
    }
    void move(bool sens, uint8_t speed,bool immediate=false) override { //immediate ignorée
       digitalWrite(_pinDIR, (sens != _inversed) ? HIGH : LOW); 
       analogWrite(_pinPWM, speed);
    }
    void stop(bool immediate=false, bool reverseSens=false) override {
      analogWrite(_pinPWM, 0);
    }  
};

class SmothMotor : public AbstractMotor, public Task { //moteur à correction de puissance et changement de vitesse progressif (`soft speed`)
/* #region(close) Attributs internes */
  protected :
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
     : AbstractMotor(TypeMotor::SMOTH, side, inversed), Task(20, true, true), //période de 20ms + isochronisme pour le `soft speed`
     _curSens(FORWARD), _targetSens(FORWARD), _curSpeed(0), _targetSpeed(0), _step(5) //configuration du `soft speed`
    {   //pré-calcul du coéfficient de compensation de force (pour l'équilibrage des roues)
        _deltaF = deltaF == 0 ? 0 : (1.0+deltaF/100.0);
        _deltaB = deltaB == 0 ? 0 : (1.0+deltaB/100.0);
    }

    void forward(uint8_t speed, bool immediate=false) override {
      _targetSens=FORWARD;
      _targetSpeed=speed;

      if(immediate){
        _curSens = FORWARD;
        digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW); 
        _curSpeed=speed;
        _applySpeed();
      }  
    }
    void backward(uint8_t speed, bool immediate=false) override {
      _targetSens=BACKWARD;
      _targetSpeed=speed;

      if(immediate){
        _curSens = BACKWARD;
        digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW); 
        _curSpeed=speed;
        _applySpeed();
      }  
    }
    void move(bool sens, uint8_t speed,bool immediate=false) override {// Vitesse de 0 à 255
      _targetSens=sens;
      _targetSpeed=speed;

      if(immediate){
        _curSens = sens;
        digitalWrite(_pinDIR, (_curSens != _inversed) ? HIGH : LOW); 
        _curSpeed=speed;
        _applySpeed();
      }  
    }
    void stop(bool immediate=false, bool reverseSens=false) override {//arrêt des moteurs
      _targetSpeed = 0;
      if(reverseSens) _targetSens=!_curSens;
      if(immediate){
        _curSpeed=0;
        analogWrite(_pinPWM, 0);
      }  
    }

    //Méthode spécifique aux moteurs smoth
    void setSmoth(uint8_t step /*max 30*/, uint32_t period=0){ //re-configuration du smothing, par défaut : 5, 20
      _step = min(step, (uint8_t) 30);
      if(period!=0) setPeriod(period);
    }
    void resetSmoth(){ //reconfigure le smothing à sa valeur par défaut : 5, 20
      _step=5;
      setPeriod(20);
    }
    void boost(uint8_t intensity=40){ //impulse un bost artificiel pour débloquer l'état statique du Robot
      _curSpeed = min(_curSpeed+intensity, 250); 
    }

/* #region(close) Méthodes internes */  
  public :   
    void init() override {
      AbstractMotor::init(); 
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
/* #region(close) Attributs internes */
  protected :
    AbstractMotor* _motorR;
    AbstractMotor* _motorL;
/* #endregion */
  
  public : //API
    SETNAME("BiMotor")
    BiMotor(AbstractMotor& motorR, AbstractMotor& motorL) : NeedInit(), 
      _motorR(&motorR), _motorL(&motorL){}
    
    void rotate(bool direction, uint8_t speed, bool immediate=false){// direction = RIGHT ou LEFT
      _motorR->move(direction, speed, immediate);
      _motorL->move(!direction, speed, immediate);
    }
    void move(bool sens, uint8_t speed,bool immediate=false){// Vitesse de 0 à 255
      _motorR->move(sens, speed, immediate);
      _motorL->move(sens, speed, immediate);
    }
    void moveLR(bool sensLeft, uint8_t speedLeft, bool sensRight, uint8_t speedRight, bool immediate=false){
      _motorR->move(sensRight, speedRight, immediate);
      _motorL->move(sensLeft, speedLeft, immediate);      
    }
    void stop(bool immediate=false, bool reverseSens=false){//arrêt des moteurs
      _motorR->stop(immediate, reverseSens);
      _motorL->stop(immediate, reverseSens); 
    }

    //Méthode spécifique aux moteurs smoth (ignoré si d'autres types de moteur sont montés)
    void setSmoth(uint8_t step /*max 30*/, uint32_t period=0){ //ignoré si le moteur n'est pas du type TypeMotor::SMOTH
       if(_motorR->type==TypeMotor::SMOTH) ((SmothMotor*) _motorR)->setSmoth(step, period);
       if(_motorL->type==TypeMotor::SMOTH) ((SmothMotor*) _motorL)->setSmoth(step, period);
    }
    void resetSmoth(){ //reconfigure le smothing à sa valeur par défaut : 5, 20, ignoré si le moteur n'est pas du type TypeMotor::SMOTH
      if(_motorR->type==TypeMotor::SMOTH) ((SmothMotor*) _motorR)->resetSmoth();
      if(_motorL->type==TypeMotor::SMOTH) ((SmothMotor*) _motorL)->resetSmoth();
    }
    void boost(uint8_t intensity=40){ //impulse un bost artificiel pour débloquer l'état statique du Robot, ignoré si le moteur n'est pas du type TypeMotor::SMOTH
      if(_motorR->type==TypeMotor::SMOTH) ((SmothMotor*) _motorR)->boost(intensity);
      if(_motorL->type==TypeMotor::SMOTH) ((SmothMotor*) _motorL)->boost(intensity);
    }
};
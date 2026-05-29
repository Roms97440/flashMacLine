//Fichier source secondaire: 
#include "Runner.h"

/*  ===> Patch en cas de blocage du robot (normalement inutile si le terrain est régulier)
  -> à activer si nécessaire
*/
//#define ENABLE_BOOST    //à commenter pour désactiver (->fixer le choix du mode en fin de code)

//=====================
#ifdef ENABLE_BOOST

#include "sensor/Ina219.h"
#include "actuator/Buzzer.h"
#include "actuator/Motor.h"


extern SensorINA219 capteurINA219;
extern Buzzer buzzer;
extern BiMotor motors; //les moteurs sont configurés dans le fichier 01_main.cpp

constexpr int seuilBoost = 300; 

//Code mono-boost
constexpr uint8_t boost = 50; 
void monoBoost(){
  if(capteurINA219.courant()>=seuilBoost){ 
      buzzer.notif(400);
      motors.boost(boost);
  }    
}

//Code boost cumulatif
constexpr uint8_t boost = 20;
constexpr uint8_t maxBoost = 3;
constexpr int seuilCumulate = seuilBoost-20;  
uint8_t count_boost=0;
void cumulBoost(){
  if(capteurINA219.courant()>= (count_boost ? seuilCumulate : seuilBoost)){
    if(count_boost<maxBoost) count_boost++;
      buzzer.notif(400);
      motors.boost(count_boost * 20);
  } else if(count_boost!=0) count_boost--; 
}

//Choix du mode
TaskCB boostTask(monoBoost,1000);
//TaskCB boostTask(cumulBoost,1000);

#endif
//=====================